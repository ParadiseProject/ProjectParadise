// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/PlayerData.h"
#include "Characters/Base/CharacterBase.h"
#include "AbilitySystemComponent.h"
#include "Components/CMP_Equipment.h"

APlayerData::APlayerData()
{
	bReplicates = false; 

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(false); // 싱글이므로 false
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	
	AttributeSet = CreateDefaultSubobject<UAttributeSet>(TEXT("AttributeSet"));


	EquipmentComponent = CreateDefaultSubobject<UCMP_Equipment>(TEXT("EquipmentComponent"));
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
