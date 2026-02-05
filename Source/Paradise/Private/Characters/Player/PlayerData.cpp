// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/PlayerData.h"
#include "Characters/Base/CharacterBase.h"
#include "GAS/Attributes/BaseAttributeSet.h"
#include "Framework/Core/ParadiseGameInstance.h"
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


	EquipmentComponent2 = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComponent"));
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
    }
}

void APlayerData::InitPlayerAssets(FCharacterAssets* Assets)
{
	if (Assets)
	{
		this->CachedMesh = Assets->SkeletalMesh.LoadSynchronous();
		this->CachedAnimBP = Assets->AnimBlueprint;
	}
}


void APlayerData::InitPlayerData(FName HeroID)
{
	UParadiseGameInstance* GI = Cast<UParadiseGameInstance>(GetGameInstance());
	if (!GI)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ [PlayerData] GameInstance를 찾을 수 없습니다."));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("🔄 [PlayerData] 영웅 초기화 시작: %s"), *HeroID.ToString());

	//스탯 데이터 조회 및 적용
	FCharacterStats* Stats = GI->GetDataTableRow<FCharacterStats>(GI->PlayerStatsDataTable, HeroID);
	if (Stats)
	{
		InitCombatAttributes(Stats);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ 스탯 데이터를 찾을 수 없습니다: %s"), *HeroID.ToString());
	}

	//에셋 데이터 조회 및 적용
	FCharacterAssets* Assets = GI->GetDataTableRow<FCharacterAssets>(GI->PlayerAssetsDataTable, HeroID);
	if (Assets)
	{
		//에셋 로드
		if (!Assets->SkeletalMesh.IsNull())
		{
			this->CachedMesh = Assets->SkeletalMesh.LoadSynchronous();
		}
		this->CachedAnimBP = Assets->AnimBlueprint;

		UE_LOG(LogTemp, Log, TEXT("✅ [PlayerData] 데이터 로드 완료"));
	}
}

void APlayerData::OnDeath()
{
	if (bIsDead) return;
	bIsDead = true;

	// 부활 타이머 시작 (예: 5초 뒤 부활)
    UE_LOG(LogTemp, Error, TEXT("👻 [PlayerData] 영혼 사망 확인. 5초 뒤 리스폰 가능합니다"));
	GetWorld()->GetTimerManager().SetTimer(
		RespawnTimerHandle, 
		this, 
		&APlayerData::OnRespawnFinished, 
		RespawnTimer, 
		false);

	UE_LOG(LogTemp, Warning, TEXT("5초 뒤 부활 예정."));
    // TODO: 여기서 GameMode나 PlayerController에게 "새 몸 줘!"라고 요청하는 코드 필요
    // 예: GetWorld()->GetAuthGameMode<AMyGameMode>()->RespawnHero(this);
}

void APlayerData::OnRespawnFinished()
{
	bIsDead = false;
	UE_LOG(LogTemp, Warning, TEXT("부활 완료! 재생성 가능."));
}
