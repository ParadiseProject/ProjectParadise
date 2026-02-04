// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/PlayerData.h"
#include "Characters/Base/CharacterBase.h"
#include "GAS/Attributes/BaseAttributeSet.h"
#include "Data/Structs/UnitStructs.h"
#include "AbilitySystemComponent.h"
#include "Components/EquipmentComponent.h"

APlayerData::APlayerData()
{
	bReplicates = false; 

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(false);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	
    CombatAttributeSet = CreateDefaultSubobject<UBaseAttributeSet>(TEXT("CombatAttributeSet"));


	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComponent"));
}



void APlayerData::InitStatsFromDataTable(const FDataTableRowHandle& InDataHandle)
{
    //핸들 저장 (나중에 참조 가능)
    CharacterStatsDataHandle = InDataHandle;

    //유효성 검사
    if (CharacterStatsDataHandle.IsNull())
    {
        UE_LOG(LogTemp, Error, TEXT("❌ [PlayerData] CharacterStatsDataHandle : 데이터 핸들이 비어있습니다!"));
        return;
    }

    //테이블에서 Row 가져오기
    //GetRow<구조체타입>(ContextString)
    static const FString ContextString(TEXT("PlayerData::InitFromDataTable"));
    FCharacterStats* Stats = CharacterStatsDataHandle.GetRow<FCharacterStats>(ContextString);

    if (Stats)
    {
        InitCombatAttributes(Stats);
        UE_LOG(LogTemp, Log, TEXT("✅ [PlayerData] 초기화 완료: %s"), *CharacterStatsDataHandle.RowName.ToString());

    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ [PlayerData] %s 행을 찾을 수 없거나 타입이 일치하지 않습니다."), *CharacterStatsDataHandle.RowName.ToString());
    }
}

void APlayerData::InitCombatAttributes(FCharacterStats* Stats)
{
    if (Stats)
    {
        //체력
        CombatAttributeSet->InitMaxHealth(Stats->BaseMaxHP);
        CombatAttributeSet->InitHealth(CombatAttributeSet->GetMaxHealth());
        //마나
        CombatAttributeSet->InitMaxMana(Stats->BaseMaxMP);
        CombatAttributeSet->InitMana(CombatAttributeSet->GetMaxMana());
        //공격력
        CombatAttributeSet->InitAttackPower(Stats->BaseAttackPower);
        //방어력
        CombatAttributeSet->InitDefense(Stats->BaseDefense);
        //크리티컬 확률
        CombatAttributeSet->InitCritRate(Stats->BaseCritRate);
        //이동 속도
        CombatAttributeSet->InitMoveSpeed(Stats->BaseMoveSpeed);
        //재사용 대기시간
        CombatAttributeSet->InitCooldown(Stats->UltimateCooldown);
    }
}

void APlayerData::InitAssetsFromDataTable(const FDataTableRowHandle& InAssetHandle)
{
    CharacterAssetsDataHandle = InAssetHandle;

    if (CharacterAssetsDataHandle.IsNull())
    {
        UE_LOG(LogTemp, Error, TEXT("❌ [PlayerData] CharacterAssetsDataHandle : 데이터 핸들이 비어있습니다!"));
        return;
    }

    //테이블에서 Row 가져오기
    //GetRow<구조체타입>(ContextString)
    static const FString ContextString(TEXT("PlayerData::InitAssetsFromDataTable"));
    FCharacterAssets* Assets = CharacterAssetsDataHandle.GetRow<FCharacterAssets>(ContextString);

    if (Assets)
    {
        this->CachedMesh = Assets->SkeletalMesh.LoadSynchronous();
        this->CachedAnimBP = Assets->AnimBlueprint;
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
