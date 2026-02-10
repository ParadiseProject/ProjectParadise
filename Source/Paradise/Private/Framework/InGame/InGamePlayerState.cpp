// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/InGame/InGamePlayerState.h"
#include "Engine/DataTable.h"
#include "Components/InventoryComponent.h"
#include "Components/EquipmentComponent.h"
#include "Components/CostManageComponent.h"
#include "Framework/Core/ParadiseGameInstance.h"
#include "Characters/Player/PlayerData.h"

AInGamePlayerState::AInGamePlayerState()
{
    InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
    CostManageComponent = CreateDefaultSubobject<UCostManageComponent>(TEXT("CostManageComponent"));
}

void AInGamePlayerState::BeginPlay()
{
    Super::BeginPlay();

	// 코스트 회복 시작
    if (CostManageComponent) CostManageComponent->StartCostRegen();
}

void AInGamePlayerState::InitSquad(const TArray<FName>& StartingHeroIDs)
{
 
    UParadiseGameInstance* GI = Cast<UParadiseGameInstance>(GetGameInstance());
    if (!GI) return;

    for (const FName& HeroID : StartingHeroIDs)
    {
        if (HeroID.IsNone()) continue;

        APlayerData* NewSoul = GetWorld()->SpawnActor<APlayerData>(PlayerDataClass);
        if (NewSoul)
        {
            //HeroID로 초기화
            NewSoul->InitPlayerData(HeroID);

            //인벤토리 연결
            if (UEquipmentComponent* EquipComp = NewSoul->GetEquipmentComponent())
            {
                EquipComp->SetLinkedInventory(this->InventoryComponent);

                UE_LOG(LogTemp, Log, TEXT("🔗 [SquadInit] %s에게 인벤토리 연결 완료"), *HeroID.ToString());
            }
            SquadMembers.Add(NewSoul);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("✅ [PlayerState] 스쿼드 초기화 완료 (%d명)"), SquadMembers.Num());
}

APlayerData* AInGamePlayerState::GetSquadMemberData(int32 Index) const
{
    if (SquadMembers.IsValidIndex(Index))
    {
        return SquadMembers[Index];
    }
    return nullptr;
}
