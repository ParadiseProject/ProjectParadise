// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/PlayerData.h"
#include "Characters/Base/CharacterBase.h"
#include "Data/Structs/UnitStructs.h"
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



void APlayerData::InitFromDataTable(const FDataTableRowHandle& InDataHandle)
{
    //핸들 저장 (나중에 참조 가능)
    CharacterDataHandle = InDataHandle;

    //유효성 검사
    if (CharacterDataHandle.IsNull())
    {
        UE_LOG(LogTemp, Error, TEXT("❌ [PlayerData] 데이터 핸들이 비어있습니다!"));
        return;
    }

    //테이블에서 Row 가져오기
    //GetRow<구조체타입>(ContextString)
    static const FString ContextString(TEXT("PlayerData::InitFromDataTable"));
    FCharacterStats* Stats = CharacterDataHandle.GetRow<FCharacterStats>(ContextString);

    if (Stats)
    {
        // 3. 스탯 적용 (예시)
        // this->MaxHP = Stats->BaseMaxHP;
        // this->AttackPower = Stats->BaseAttackPower;

        // 이름 설정 (디버그용)
        // SetActorLabel(CharacterDataHandle.RowName.ToString()); 

        UE_LOG(LogTemp, Log, TEXT("✅ [PlayerData] 초기화 완료: %s (HP: %f)"), *CharacterDataHandle.RowName.ToString(), Stats->BaseMaxHP);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ [PlayerData] %s 행을 찾을 수 없거나 타입이 일치하지 않습니다."), *CharacterDataHandle.RowName.ToString());
    }
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
