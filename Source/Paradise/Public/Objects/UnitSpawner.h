// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h" // 데이터 테이블 사용을 위해 추가
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
	FVector GetRandomSpawnLocation();

public:
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class ABaseUnit> UnitClass;

	/** @brief 사용할 스탯 데이터 테이블 */
	UPROPERTY(EditAnywhere, Category = "Spawning|Data")
	UDataTable* StatsDataTable;

	/** @brief 사용할 에셋 데이터 테이블 */
	UPROPERTY(EditAnywhere, Category = "Spawning|Data")
	UDataTable* AssetsDataTable;

	/** @brief 기존 행 이름 변수 유지 */
	UPROPERTY(EditAnywhere, Category = "Spawning|Data")
	FName EnemyRowName;

	/** @brief 사각형 스폰 범위 */
	UPROPERTY(EditAnywhere, Category = "Spawning|Area", meta = (MakeEditWidget = true))
	FVector SpawnExtent = FVector(500.0f, 500.0f, 0.0f);

	UPROPERTY(EditAnywhere, Category = "Spawning")
	int32 PreSpawnCount = 10;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	float SpawnInterval = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	int32 TeamID = 2;

private:
	FTimerHandle SpawnTimerHandle;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};