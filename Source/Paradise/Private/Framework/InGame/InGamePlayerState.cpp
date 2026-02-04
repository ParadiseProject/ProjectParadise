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

        UClass* SpawnClass = nullptr;
        if(PlayerDataClass) {
            SpawnClass = PlayerDataClass;
            UE_LOG(LogTemp, Warning, TEXT("⚠️ [PlayerState] PlayerDataClass로 스폰되었습니다."));
        }
        else {
            SpawnClass= APlayerData::StaticClass();
            UE_LOG(LogTemp, Warning, TEXT("⚠️ [PlayerState] PlayerDataClass가 설정되지 않았습니다. 기본 C++ 클래스로 스폰합니다."));
        }

        APlayerData* NewSoul = GetWorld()->SpawnActor<APlayerData>(
            SpawnClass,
            SpawnParams);

        if (NewSoul)
        {
            //스탯데이터핸들 생성 및 ID 주입
            FDataTableRowHandle StatDataHandle;
            StatDataHandle.DataTable = PlayerStatDataTable; //에디터에서 지정한 테이블 사용
            StatDataHandle.RowName = HeroID;

            //에셋데이터핸들 생성 및 ID 주입
            FDataTableRowHandle AssetDataHandle;
            AssetDataHandle.DataTable = PlayerAssetDataTable; //에디터에서 지정한 테이블 사용
            AssetDataHandle.RowName = HeroID;

            //PlayerData 초기화
            NewSoul->InitStatsFromDataTable(StatDataHandle);
            NewSoul->InitAssetsFromDataTable(AssetDataHandle);
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
