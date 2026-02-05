// Fill out your copyright notice in the Description page of Project Settings.

#include "Objects/UnitSpawner.h"
#include "Characters/AIUnit/BaseUnit.h"
#include "BrainComponent.h"
#include "AIController.h"
#include "Data/Structs/UnitStructs.h"

AUnitSpawner::AUnitSpawner() { PrimaryActorTick.bCanEverTick = false; }

void AUnitSpawner::BeginPlay()
{
    Super::BeginPlay();

    for (int32 i = 0; i < PreSpawnCount; i++)
    {
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        ABaseUnit* NewUnit = GetWorld()->SpawnActor<ABaseUnit>(UnitClass, GetActorLocation(), GetActorRotation(), Params);
        if (NewUnit)
        {
            NewUnit->TeamID = TeamID;
            NewUnit->SetActorHiddenInGame(true);
            NewUnit->SetActorEnableCollision(false);
            NewUnit->SetActorTickEnabled(false);

            if (AAIController* AIC = Cast<AAIController>(NewUnit->GetController()))
            {
                if (AIC->GetBrainComponent()) AIC->GetBrainComponent()->StopLogic("Initial");
            }

            UnitPool.Add(NewUnit);
        }
    }

    GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AUnitSpawner::SpawnUnit, SpawnInterval, true);
}

void AUnitSpawner::SpawnUnit()
{
    for (ABaseUnit* Unit : UnitPool)
    {
        if (Unit && Unit->IsHidden())
        {
            // 데이터테이블에서 정보 가져오기
            if (!UnitDataRow.IsNull())
            {
                FCharacterStats* Stats = UnitDataRow.GetRow<FCharacterStats>(TEXT("UnitSpawnContext"));
                if (Stats)
                {
                    Unit->InitializeUnit(Stats); // 유닛 스탯 초기화
                }
            }

            // 활성화 로직
            Unit->SetActorLocation(GetActorLocation());
            Unit->SetActorHiddenInGame(false);
            Unit->SetActorEnableCollision(true);
            Unit->SetActorTickEnabled(true);

            if (AAIController* AIC = Cast<AAIController>(Unit->GetController()))
            {
                if (AIC->GetBrainComponent()) AIC->GetBrainComponent()->RestartLogic();
            }

            return;
        }
    }
}