// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "UnitSpawner.generated.h"

UCLASS()
class PARADISE_API AUnitSpawner : public AActor
{
    GENERATED_BODY()

public:
    AUnitSpawner();

protected:
    virtual void BeginPlay() override;
    void SpawnUnit();

public:
    UPROPERTY(EditAnywhere, Category = "Spawning")
    TSubclassOf<class ABaseUnit> UnitClass;

    // 선택할 데이터테이블 행
    UPROPERTY(EditAnywhere, Category = "Spawning")
    FDataTableRowHandle UnitDataRow;

    UPROPERTY(EditAnywhere, Category = "Spawning")
    int32 PreSpawnCount = 20;

    UPROPERTY(EditAnywhere, Category = "Spawning")
    float SpawnInterval = 3.0f;

    UPROPERTY(EditAnywhere, Category = "Spawning")
    int32 TeamID = 1;

private:
    UPROPERTY()
    TArray<class ABaseUnit*> UnitPool;

    FTimerHandle SpawnTimerHandle;
};