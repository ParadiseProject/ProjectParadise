// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Base/CharacterBase.h"
#include "Framework/System/ObjectPoolSubsystem.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AttributeSet.h"

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	HealthWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamageWidget"));
	HealthWidget->SetupAttachment(RootComponent);

	HealthWidget->SetWidgetSpace(EWidgetSpace::Screen);
	//UI 만들어지면 사이즈 조정 예정
	HealthWidget->SetDrawSize(FVector2D(300.0f,50.0f));

}


void ACharacterBase::TestKillSelf()
{
	if (bIsDead)
	{
		UE_LOG(LogTemp, Warning, TEXT("⚠️ [Debug] 이미 사망한 상태입니다."));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("💀 [Debug] 강제 사망 명령 실행! (TestKillSelf)"));
	Die();
}

void ACharacterBase::CheckHit(FName SocketName, float AttackRadius)
{
	FVector TraceStart;

	// 1. 소켓 위치 찾기 시도
	if (GetMesh()->DoesSocketExist(SocketName))
	{
		TraceStart = GetMesh()->GetSocketLocation(SocketName);
	}
	else
	{
		// ⚠️ 예외 처리: 소켓이 없거나 이름이 틀렸을 때
		// 캐릭터의 위치 + 전방 100cm 앞을 타격 지점으로 설정
		TraceStart = GetActorLocation() + (GetActorForwardVector() * 100.0f);

		// 디버그용 로그 (개발 중에만 켜두세요)
		// UE_LOG(LogTemp, Warning, TEXT("[%s] 소켓(%s)을 찾을 수 없어 전방 위치를 사용합니다."), *GetName(), *SocketName.ToString());
	}

	// 2. 트레이스 설정 (Multi로 변경하여 다수 타격 지원)
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this); // 나는 때리면 안 됨

	TArray<FHitResult> HitResults;
	bool bHit = UKismetSystemLibrary::SphereTraceMulti(
		GetWorld(),
		TraceStart,      // 시작점
		TraceStart,      // 끝점 (제자리 구체)
		AttackRadius,    // 반경 (인자로 받음)
		UEngineTypes::ConvertToTraceType(ECC_Pawn), // 폰만 검사
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration, // 디버그 선 그리기
		HitResults,
		true
	);

	// 3. 결과 처리
	if (bHit)
	{
		for (const FHitResult& Result : HitResults)
		{
			AActor* HitActor = Result.GetActor();
			if (!HitActor) continue;

			// 중복 타격 방지
			if (HitActors.Contains(HitActor)) continue;
			HitActors.Add(HitActor);

			// 태그 기반 피아 식별
			if (ACharacterBase* HitChar = Cast<ACharacterBase>(HitActor))
			{
				if (!IsHostile(HitChar))
				{
					continue;
				}
			}

			// GAS 이벤트 전송
			FGameplayEventData Payload;
			Payload.Instigator = this;
			Payload.Target = HitActor;

			// 태그를 고정하거나, 인자로 받을 수도 있음
			FGameplayTag HitTag = FGameplayTag::RequestGameplayTag(FName("Event.Montage.Hit"));

			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, HitTag, Payload);

			UE_LOG(LogTemp, Log, TEXT("⚔️ [%s] 타격 성공! 대상: %s (소켓: %s)"), *GetName(), *HitActor->GetName(), *SocketName.ToString());
		}
	}
}

void ACharacterBase::ResetHitActors()
{
	HitActors.Empty();
}

bool ACharacterBase::IsHostile(ACharacterBase* Target) const
{
	if (!Target) return false;

	// 🚨 [디버그용 로그] 누구와 누구의 태그를 비교하는지 확인
	UE_LOG(LogTemp, Warning, TEXT("⚔️ [IsHostile 판정] 공격자(%s) 태그: %s  VS  피격자(%s) 태그: %s"),
		*GetName(), *this->FactionTag.ToString(),
		*Target->GetName(), *Target->FactionTag.ToString());

	// 태그가 완전히 똑같으면 무조건 아군
	if (this->FactionTag == Target->FactionTag) return false;

	// 상위 태그(부모 태그)를 기준으로 그룹 검사
	FGameplayTag FriendlyTag = FGameplayTag::RequestGameplayTag("Unit.Faction.Friendly");

	bool bAmIFriendly = this->FactionTag.MatchesTag(FriendlyTag);
	bool bIsTargetFriendly = Target->FactionTag.MatchesTag(FriendlyTag);

	// Friendly 그룹이고 상대도 Friendly 그룹이면 false (아군)
	// Friendly 그룹인데 상대가 아니면(Enemy면) true (적군)
	return bAmIFriendly != bIsTargetFriendly;
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (CurrentWeaponActor)
	{
		CurrentWeaponActor->Destroy();
		CurrentWeaponActor = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}


void ACharacterBase::AttachWeapon(AActor* NewWeapon, FName SocketName)
{
	//기존 무기 정리
	if (CurrentWeaponActor)
	{
		CurrentWeaponActor->Destroy();
		CurrentWeaponActor = nullptr;
	}

	if (!NewWeapon || !GetMesh()) return;

	//새 무기 등록
	CurrentWeaponActor = NewWeapon;

	//소켓에 부착 (SnapToTarget: 위치/회전/크기 모두 소켓 기준)
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
	CurrentWeaponActor->AttachToComponent(GetMesh(), AttachRules, SocketName);

	//소유자 설정 (GAS 데미지 계산 시 Instigator로 활용됨)
	CurrentWeaponActor->SetOwner(this);

	UE_LOG(LogTemp, Warning, TEXT("⚔️ [CharacterBase] 무기 장착 완료: %s -> 소켓: %s"),
		*NewWeapon->GetName(), *SocketName.ToString());
}

void ACharacterBase::PlayHitFlash()
{
	if (USkeletalMeshComponent* MyMesh = GetMesh())
	{
		//0번인덱스의 커스텀 프리미티브 데이터 1.0f 로 변경 //intensity
		MyMesh->SetCustomPrimitiveDataFloat(0, 1.0f);
		//1~3번인덱스의 벡터값 변경 //Red로 변하게
		MyMesh->SetCustomPrimitiveDataVector3(1, FVector(1.0f, 0.0f, 0.0f));

		//4번인덱스의 float값 변경 //투명도
		MyMesh->SetCustomPrimitiveDataFloat(4, 100.0f);
	}

	//3초후 이펙트 리셋 함수호출
	GetWorldTimerManager().SetTimer(
		HitEffectTimerHandle,
		this,
		&ACharacterBase::ResetHitFlash,
		HitResetTime,
		false
	);
}

void ACharacterBase::ResetHitFlash()
{
	if (USkeletalMeshComponent* MyMesh = GetMesh())
	{
		//0번인덱스의 커스텀 프리미티브 데이터 0.0f 로 리셋
		MyMesh->SetCustomPrimitiveDataFloat(0, 0.0f);

		//1~3번인덱스의 벡터값 변경 //Red로 변하게
		MyMesh->SetCustomPrimitiveDataVector3(1, FVector(0.0f, 0.0f, 0.0f));

		//4번인덱스의 float값 변경 //투명도
		MyMesh->SetCustomPrimitiveDataFloat(4, 0.0f);
	}
}

void ACharacterBase::SpawnDamagePopup(float DamageAmount)
{
	if (DamageAmount <= 0.0f) return;

	UWorld* world = GetWorld();

	if (!world) return;

	UObjectPoolSubsystem* subsystem = world->GetSubsystem<UObjectPoolSubsystem>();

	if (subsystem && DamageTextActorClass)
	{
		//스폰 및 데미지 수치 전달 로직 
		//subsystem->SpawnPoolActor<>();
	}



}

void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACharacterBase::Die()
{
	if (bIsDead) return;
	bIsDead = true;

	UE_LOG(LogTemp, Error, TEXT("☠️ [CharacterBase] Die() 로직 시작 - 래그돌 전환"));

	//물리적 처리 (서 있는 캡슐은 끄고, 메쉬는 흐물거리는 래그돌로)
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (GetMesh())
	{
		// 래그돌 프리셋 적용 (PhysicsAsset이 설정되어 있어야 함)
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetSimulatePhysics(true);
	}

	//조작 차단
	if (Controller)
	{
		Controller->UnPossess(); // 영혼 이탈
	}

	//시체 청소 (5초 뒤에 액터 삭제)
	SetLifeSpan(5.0f);
}
