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

		FFindFloorResult FloorResult;
		MoveComp->FindFloor(MoveComp->UpdatedComponent->GetComponentLocation(), FloorResult, false);
	}
}

void ABaseUnit::OnPoolDeactivate_Implementation()
{
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

void ABaseUnit::InitializeUnit(FEnemyStats* InStats, FEnemyAssets* InAssets)
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
		SetActorScale3D(FVector(InAssets->Scale));
		if (!InAssets->SkeletalMesh.IsNull())
		{
			USkeletalMesh* LoadedMesh = InAssets->SkeletalMesh.LoadSynchronous();
			if (LoadedMesh) GetMesh()->SetSkeletalMesh(LoadedMesh);
		}
		if (InAssets->AnimBlueprint)
		{
			GetMesh()->SetAnimInstanceClass(InAssets->AnimBlueprint);
		}
	}
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
			PoolSubsystem->ReturnToPool(this);
		}
	}
}

bool ABaseUnit::IsEnemy(ABaseUnit* OtherUnit)
{
	if (!OtherUnit || OtherUnit == this) return false;
	return !this->FactionTag.MatchesTag(OtherUnit->FactionTag);
}