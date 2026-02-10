// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/Structs/UnitStructs.h"
#include "UnitSpawner.generated.h"

/** @brief 웨이브 설정을 위한 구조체 */
USTRUCT(BlueprintType)
struct FWaveConfig
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	FName UnitRowName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	int32 SpawnCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float SpawnInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float NextWaveDelay;

	FWaveConfig()
		: UnitRowName(NAME_None), SpawnCount(10), SpawnInterval(1.0f), NextWaveDelay(5.0f)
	{
	}
};

UCLASS()
class PARADISE_API AUnitSpawner : public AActor
{
	GENERATED_BODY()

public:
	AUnitSpawner();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class ABaseUnit> UnitClass;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	class UDataTable* StatsDataTable;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	class UDataTable* AssetsDataTable;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	FVector SpawnExtent = FVector(500.f, 500.f, 0.f);

	/** ⭐ 이제 UHT가 FWaveConfig를 정상적으로 인식합니다 */
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TArray<FWaveConfig> WaveConfigs;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	int32 PreSpawnCount = 5;

	FTimerHandle SpawnTimerHandle;
	int32 CurrentWaveIndex = 0;
	int32 CurrentSpawnCountInWave = 0;
	FName EnemyRowName;

	void SpawnUnit();
	FVector GetRandomSpawnLocation();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};