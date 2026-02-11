// Fill out your copyright notice in the Description page of Project Settings.

#include "Objects/UnitSpawner.h"
#include "Characters/AIUnit/BaseUnit.h"
#include "Framework/System/ObjectPoolSubsystem.h"
#include "Framework/InGame/MyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"

AUnitSpawner::AUnitSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AUnitSpawner::BeginPlay()
{
	Super::BeginPlay();

	UObjectPoolSubsystem* PoolSubsystem = GetWorld()->GetSubsystem<UObjectPoolSubsystem>();
	if (PoolSubsystem && UnitClass)
	{
		for (int32 i = 0; i < PreSpawnCount; i++)
		{
			ABaseUnit* TempUnit = PoolSubsystem->SpawnPoolActor<ABaseUnit>(UnitClass, GetActorLocation(), GetActorRotation(), this, nullptr);
			if (TempUnit) PoolSubsystem->ReturnToPool(TempUnit);
		}
	}

	if (WaveConfigs.Num() > 0)
	{
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AUnitSpawner::SpawnUnit, WaveConfigs[0].SpawnInterval, true, 1.0f);
	}
}

void AUnitSpawner::SpawnUnit()
{
	if (!WaveConfigs.IsValidIndex(CurrentWaveIndex))
	{
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
		return;
	}

	EnemyRowName = WaveConfigs[CurrentWaveIndex].UnitRowName;
	UObjectPoolSubsystem* PoolSubsystem = GetWorld()->GetSubsystem<UObjectPoolSubsystem>();

	if (!PoolSubsystem || !UnitClass || !StatsDataTable || !AssetsDataTable || EnemyRowName.IsNone())
	{
		return;
	}

	FVector SpawnLocation = GetRandomSpawnLocation() + FVector(0.f, 0.f, 100.0f);
	FRotator SpawnRotation = FRotator(0.f, FMath::RandRange(0.f, 360.f), 0.f);

	ABaseUnit* NewUnit = PoolSubsystem->SpawnPoolActor<ABaseUnit>(UnitClass, SpawnLocation, SpawnRotation, this, nullptr);

	if (NewUnit)
	{
		NewUnit->SetActorLocationAndRotation(SpawnLocation, SpawnRotation, false, nullptr, ETeleportType::ResetPhysics);

		// 스폰된 유닛에게 UnitID 부여
		NewUnit->SetUnitID(EnemyRowName);

		FEnemyStats* StatData = StatsDataTable->FindRow<FEnemyStats>(EnemyRowName, TEXT(""));
		FEnemyAssets* AssetData = AssetsDataTable->FindRow<FEnemyAssets>(EnemyRowName, TEXT(""));

		if (StatData && AssetData)
		{
			NewUnit->InitializeUnit(StatData, AssetData);

			// AI 컨트롤러 확인
			AMyAIController* AIC = Cast<AMyAIController>(NewUnit->GetController());

			// 컨트롤러가 없다면 생성
			if (!AIC)
			{
				NewUnit->SpawnDefaultController();
				AIC = Cast<AMyAIController>(NewUnit->GetController());
			}

			if (AIC)
			{
				/** * [핵심 수정] AIC->Possess(NewUnit) 수동 호출을 제거합니다.
				 */

				 // 1. 블랙보드에 데이터 테이블 스탯 주입
				AIC->LoadUnitStatsFromTable();

				// 2. 비헤이비어 트리 실행
				if (!AssetData->BehaviorTree.IsNull())
				{
					UBehaviorTree* BT = AssetData->BehaviorTree.LoadSynchronous();
					if (BT)
					{
						AIC->RunBehaviorTree(BT);
						UE_LOG(LogTemp, Log, TEXT("🚀 [%s] Behavior Tree Started Successfully."), *NewUnit->GetName());
					}
				}
			}
		}
	}

	CurrentSpawnCountInWave++;
	if (CurrentSpawnCountInWave >= WaveConfigs[CurrentWaveIndex].SpawnCount)
	{
		CurrentSpawnCountInWave = 0;
		int32 FinishedIdx = CurrentWaveIndex;
		CurrentWaveIndex++;

		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);

		if (WaveConfigs.IsValidIndex(CurrentWaveIndex))
		{
			float NextDelay = WaveConfigs[FinishedIdx].NextWaveDelay;
			GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AUnitSpawner::SpawnUnit,
				WaveConfigs[CurrentWaveIndex].SpawnInterval, true, NextDelay);
		}
	}
}

FVector AUnitSpawner::GetRandomSpawnLocation()
{
	FVector Origin = GetActorLocation();
	FVector TargetPoint = Origin + FVector(FMath::RandRange(-SpawnExtent.X, SpawnExtent.X), FMath::RandRange(-SpawnExtent.Y, SpawnExtent.Y), 0.0f);

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	FNavLocation NavLocation;
	if (NavSys && NavSys->ProjectPointToNavigation(TargetPoint, NavLocation, FVector(0.f, 0.f, 500.f)))
	{
		return NavLocation.Location;
	}
	return TargetPoint;
}

#if WITH_EDITOR
void AUnitSpawner::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	DrawDebugBox(GetWorld(), GetActorLocation(), SpawnExtent, FColor::Cyan, false, 2.0f, 0, 5.0f);
}
#endif