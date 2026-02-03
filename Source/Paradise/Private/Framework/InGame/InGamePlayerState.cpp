// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/InGame/InGamePlayerState.h"
#include "Engine/DataTable.h"
#include "Characters/Player/PlayerData.h"

AInGamePlayerState::AInGamePlayerState()
{
}

void AInGamePlayerState::InitSquad(const TArray<FName>& StartingHeroIDs)
{
    UE_LOG(LogTemp, Warning, TEXT("[PlayerState] 스쿼드 초기화 시작"));

    //전달받은 ID 개수만큼 반복 (없으면 생성 안 함)
    for (const FName& HeroID : StartingHeroIDs)
    {
        if (HeroID.IsNone()) continue;

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        APlayerData* NewSoul = GetWorld()->SpawnActor<APlayerData>(APlayerData::StaticClass(), SpawnParams);

        if (NewSoul)
        {
            //핸들 생성 및 ID 주입
            FDataTableRowHandle DataHandle;
            DataHandle.DataTable = PlayerDataTable; //에디터에서 지정한 테이블 사용
            DataHandle.RowName = HeroID;

            //PlayerData 초기화
            NewSoul->InitFromDataTable(DataHandle);

            //관리 목록 추가
            SquadMembers.Add(NewSoul);
        }
    }

    //전달 받은 멤버 숫자가 아무도없으면 로그
    if (SquadMembers.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("⚠️ 전달된 유효한 ID가 없어 스쿼드가 비어 있습니다. 더미 데이터를 생성하지 않습니다."));
    }

    UE_LOG(LogTemp, Warning, TEXT("✅ [PlayerState] 스쿼드 생성 완료! (멤버 수: %d)"), SquadMembers.Num());
}

APlayerData* AInGamePlayerState::GetSquadMemberData(int32 Index) const
{
    if (SquadMembers.IsValidIndex(Index))
    {
        return SquadMembers[Index];
    }
    return nullptr;
}
