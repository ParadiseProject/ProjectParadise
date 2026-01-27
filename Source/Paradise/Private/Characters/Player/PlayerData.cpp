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
