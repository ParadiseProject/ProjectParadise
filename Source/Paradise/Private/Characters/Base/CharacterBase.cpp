// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Base/CharacterBase.h"
#include "Framework/System/ObjectPoolSubsystem.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
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
