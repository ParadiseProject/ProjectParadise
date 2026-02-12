// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/InGame/InGamePlayerState.h"
#include "Engine/DataTable.h"
#include "Framework/System/InventorySystem.h"
#include "Components/EquipmentComponent.h"
#include "Components/CostManageComponent.h"
#include "Components/FamiliarSummonComponent.h"
#include "Framework/Core/ParadiseGameInstance.h"
#include "Characters/Player/PlayerData.h"

AInGamePlayerState::AInGamePlayerState()
{
    CostManageComponent = CreateDefaultSubobject<UCostManageComponent>(TEXT("CostManageComponent"));
    FamiliarSummonComponent = CreateDefaultSubobject<UFamiliarSummonComponent>(TEXT("FamiliarSummonComponent"));
}

void AInGamePlayerState::BeginPlay()
{
    Super::BeginPlay();

	// 코스트 회복 시작
    if (CostManageComponent) CostManageComponent->StartCostRegen();
}

void AInGamePlayerState::InitSquad(const TArray<FName>& StartingHeroIDs)
{
	//인벤토리 시스템 가져오기
	UInventorySystem* InvSys = GetInventorySystem();
	if (!InvSys)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ [SquadInit] 인벤토리 시스템을 찾을 수 없습니다!"));
		return;
	}

	for (const FName& HeroID : StartingHeroIDs)
	{
		if (HeroID.IsNone()) continue;

		UClass* SpawnClass = nullptr;
		//(PlayerData) 스폰
		if (PlayerDataClass)
		{
			SpawnClass = PlayerDataClass;
		}
		else {
			SpawnClass= APlayerData::StaticClass();
		}
		 
		APlayerData* NewSoul = GetWorld()->SpawnActor<APlayerData>(SpawnClass);

		if (NewSoul)
		{
			NewSoul->InitPlayerData(HeroID);

			//장비 컴포넌트 초기화
			if (UEquipmentComponent* EquipComp = NewSoul->GetEquipmentComponent())
			{
				//데이터 검색
				if (const FOwnedCharacterData* CharData = InvSys->GetCharacterDataByID(HeroID))
				{
					// 찾은 데이터의 장비 맵으로 초기화
					EquipComp->InitializeEquipment(CharData->EquipmentMap);

					UE_LOG(LogTemp, Log, TEXT("🔗 [SquadInit] %s 장비 데이터 동기화 완료"), *HeroID.ToString());
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("⚠️ [SquadInit] 인벤토리에 %s 데이터가 없습니다."), *HeroID.ToString());
				}
			}

			SquadMembers.Add(NewSoul);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("✅ [PlayerState] 스쿼드 초기화 완료 (%d명)"), SquadMembers.Num());
}

UInventorySystem* AInGamePlayerState::GetInventorySystem() const
{
    if (UGameInstance* GI = GetWorld()->GetGameInstance())
    {
        return GI->GetSubsystem<UInventorySystem>();
    }
    return nullptr;
}



APlayerData* AInGamePlayerState::GetSquadMemberData(int32 Index) const
{
    if (SquadMembers.IsValidIndex(Index))
    {
        return SquadMembers[Index];
    }
    return nullptr;
}
