// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "UnitSpawner.generated.h"

/** @brief 웨이브별 설정을 위한 구조체 */
USTRUCT(BlueprintType)
struct FUnitWaveInfo
{
	GENERATED_BODY()

	/** @brief 데이터 테이블에서 가져올 행 이름 (기존 EnemyRowName에 할당됨) */
	UPROPERTY(EditAnywhere, Category = "Wave")
	FName UnitRowName;

	/** @brief 이번 웨이브에 스폰할 총 마릿수 */
	UPROPERTY(EditAnywhere, Category = "Wave")
	int32 SpawnCount = 5;

	/** @brief 유닛 간 스폰 간격 (초) */
	UPROPERTY(EditAnywhere, Category = "Wave")
	float SpawnInterval = 2.0f;

	/** @brief 현재 웨이브 종료 후 다음 웨이브 시작까지의 대기 시간 */
	UPROPERTY(EditAnywhere, Category = "Wave")
	float NextWaveDelay = 10.0f;
};

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

	UPROPERTY(EditAnywhere, Category = "Spawning|Data")
	UDataTable* StatsDataTable;

	UPROPERTY(EditAnywhere, Category = "Spawning|Data")
	UDataTable* AssetsDataTable;

	/** @brief 기존 행 이름 변수 유지 (웨이브 진행 시 자동으로 값이 바뀜) */
	UPROPERTY(VisibleAnywhere, Category = "Spawning|Data")
	FName EnemyRowName;

	/** @brief 설계도: 에디터에서 순서대로 유닛 이름과 마릿수 설정 */
	UPROPERTY(EditAnywhere, Category = "Spawning|Wave")
	TArray<FUnitWaveInfo> WaveConfigs;

	UPROPERTY(EditAnywhere, Category = "Spawning|Area", meta = (MakeEditWidget = true))
	FVector SpawnExtent = FVector(500.0f, 500.0f, 0.0f);

	UPROPERTY(EditAnywhere, Category = "Spawning")
	int32 PreSpawnCount = 10;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	int32 TeamID = 2;

private:
	FTimerHandle SpawnTimerHandle;

	// 웨이브 관리용 변수
	int32 CurrentWaveIndex = 0;
	int32 CurrentSpawnCountInWave = 0;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};