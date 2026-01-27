// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Base/CharacterBase.h"
#include "Framework/System/ObjectPoolSubsystem.h"
#include "Components/WidgetComponent.h"
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
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

float ACharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (DamageAmount <= 0) return 0.0f;

	PlayHitFlash();

	SpawnDamagePopup(DamageAmount);

	return DamageAmount;
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
		MyMesh->SetCustomPrimitiveDataFloat(4, 10.0f);
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

