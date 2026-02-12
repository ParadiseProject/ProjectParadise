// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/AIUnit/BaseUnit.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Framework/System/ObjectPoolSubsystem.h"

ABaseUnit::ABaseUnit()
{
	PrimaryActorTick.bCanEverTick = false;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	bIsDead = false;
}

void ABaseUnit::OnPoolActivate_Implementation()
{
	bIsDead = false;
	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);
	SetActorEnableCollision(true);

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->Velocity = FVector::ZeroVector;
		MoveComp->SetMovementMode(MOVE_Walking);
	}
}

void ABaseUnit::OnPoolDeactivate_Implementation()
{
	// 풀로 돌아갈 때 AI 로직 중지 및 컨트롤러 해제
	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		if (AIC->GetBrainComponent())
		{
			AIC->GetBrainComponent()->StopLogic("Returned to Pool");
		}
		AIC->UnPossess();
	}

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
}

void ABaseUnit::InitializeUnit(FAIUnitStats* InStats, FAIUnitAssets* InAssets)
{
	if (InStats)
	{
		MaxHP = InStats->BaseMaxHP;
		HP = MaxHP;
		this->FactionTag = InStats->FactionTag;

		if (GetCharacterMovement())
		{
			GetCharacterMovement()->MaxWalkSpeed = InStats->BaseMoveSpeed;
		}
	}

	if (InAssets)
	{
		// 유닛 크기 설정
		SetActorScale3D(FVector(InAssets->Scale));

		// 스켈레탈 메시 로드 및 적용
		if (!InAssets->SkeletalMesh.IsNull())
		{
			USkeletalMesh* LoadedMesh = InAssets->SkeletalMesh.LoadSynchronous();
			if (LoadedMesh) GetMesh()->SetSkeletalMesh(LoadedMesh);
		}

		// 애니메이션 블루프린트 설정
		if (InAssets->AnimBlueprint)
		{
			GetMesh()->SetAnimInstanceClass(InAssets->AnimBlueprint);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[%s] Initialized. Faction: %s"), *GetName(), *FactionTag.ToString());
}

float ABaseUnit::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead) return 0.0f;

	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	HP -= ActualDamage;

	if (HP <= 0.0f)
	{
		bIsDead = true;
		Die();
	}
	return ActualDamage;
}

void ABaseUnit::Die()
{
	if (UWorld* World = GetWorld())
	{
		if (UObjectPoolSubsystem* PoolSubsystem = World->GetSubsystem<UObjectPoolSubsystem>())
		{
			// 사망 시 풀로 반환
			PoolSubsystem->ReturnToPool(this);
		}
	}
}

bool ABaseUnit::IsEnemy(ABaseUnit* OtherUnit)
{
	if (!OtherUnit || OtherUnit == this) return false;
	// 태그가 다르면 적군으로 간주
	return !this->FactionTag.MatchesTag(OtherUnit->FactionTag);
}

void ABaseUnit::PlayRangeAttack()
{
	// 공격 몽타주 실행 또는 발사체 생성 로직
	UE_LOG(LogTemp, Log, TEXT("%s 유닛이 원거리 공격을 수행합니다."), *GetName());
}