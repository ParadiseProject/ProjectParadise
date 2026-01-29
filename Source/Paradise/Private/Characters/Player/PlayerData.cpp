// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/PlayerData.h"
#include "Characters/Base/CharacterBase.h"
#include "AbilitySystemComponent.h"
#include "Components/EquipmentComponent.h"

APlayerData::APlayerData()
{
	bReplicates = false; 

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(false); // 싱글이므로 false
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	
	AttributeSet = CreateDefaultSubobject<UAttributeSet>(TEXT("AttributeSet"));


	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComponent"));
}

void APlayerData::InitFromDataAsset(UPrimaryDataAsset* InData)
{
	this->RespawnTimer = 3.0f; // 3초 부활

	// 로그 출력
	FString DummyName = FString::Printf(TEXT("Hero_%d"), FMath::RandRange(100, 999)); // 임시 이름
	UE_LOG(LogTemp, Warning, TEXT("👻 [PlayerData : %s] 영혼 생성 완료! (이름: %s)"),*this->GetName(), *DummyName);
}

void APlayerData::OnDeath()
{
	if (bIsDead) return;
	bIsDead = true;

	// 부활 타이머 시작 (예: 5초 뒤 부활)
	GetWorld()->GetTimerManager().SetTimer(
		RespawnTimerHandle, 
		this, 
		&APlayerData::OnRespawnFinished, 
		RespawnTimer, 
		false);

	UE_LOG(LogTemp, Warning, TEXT("5초 뒤 부활 예정."));
}

void APlayerData::OnRespawnFinished()
{
	bIsDead = false;
	UE_LOG(LogTemp, Warning, TEXT("부활 완료! 재생성 가능."));
}
