// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/InGame/InGamePlayerState.h"
#include "Engine/DataTable.h"
#include "Framework/Core/ParadiseGameInstance.h"
#include "Characters/Player/PlayerData.h"

AInGamePlayerState::AInGamePlayerState()
{
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
            //HeroID 로 GI 에서 조회해서 초기화
            NewSoul->InitPlayerData(HeroID);

            // 스쿼드에 추가
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
