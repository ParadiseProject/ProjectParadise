// Fill out your copyright notice in the Description page of Project Settings.

#include "Objects/UnitSpawner.h"
#include "Characters/AIUnit/BaseUnit.h"
#include "Framework/System/ObjectPoolSubsystem.h"
#include "AIController.h"
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

	// [기존 로직 유지] 프리 스폰 (오브젝트 풀 예열)
	UObjectPoolSubsystem* PoolSubsystem = GetWorld()->GetSubsystem<UObjectPoolSubsystem>();
	if (PoolSubsystem && UnitClass)
	{
		for (int32 i = 0; i < PreSpawnCount; i++)
		{
			ABaseUnit* TempUnit = PoolSubsystem->SpawnPoolActor<ABaseUnit>(UnitClass, GetActorLocation(), GetActorRotation(), this, nullptr);
			if (TempUnit) PoolSubsystem->ReturnToPool(TempUnit);
		}
	}

	// [기존 로직 유지] 첫 웨이브 타이머 시작
	if (WaveConfigs.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Starting First Wave..."));
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AUnitSpawner::SpawnUnit, WaveConfigs[0].SpawnInterval, true, 1.0f);
	}
}

void AUnitSpawner::SpawnUnit()
{
	// 1. 웨이브 인덱스 체크
	if (!WaveConfigs.IsValidIndex(CurrentWaveIndex))
	{
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
		return;
	}

	// 2. 현재 웨이브의 유닛 정보 가져오기
	EnemyRowName = WaveConfigs[CurrentWaveIndex].UnitRowName;
	UObjectPoolSubsystem* PoolSubsystem = GetWorld()->GetSubsystem<UObjectPoolSubsystem>();

	if (!PoolSubsystem || !UnitClass || !StatsDataTable || !AssetsDataTable || EnemyRowName.IsNone())
	{
		UE_LOG(LogTemp, Error, TEXT("Spawner Check Failed! RowName: %s"), *EnemyRowName.ToString());
		return;
	}

	// 3. 스폰 위치 및 생성
	FVector SpawnLocation = GetRandomSpawnLocation() + FVector(0.f, 0.f, 100.0f);
	FRotator SpawnRotation = FRotator(0.f, FMath::RandRange(0.f, 360.f), 0.f);

	ABaseUnit* NewUnit = PoolSubsystem->SpawnPoolActor<ABaseUnit>(UnitClass, SpawnLocation, SpawnRotation, this, nullptr);

	if (NewUnit)
	{
		// 풀에서 막 나온 유닛 위치/회전 강제 재설정 (기존 기능 유지)
		NewUnit->SetActorLocationAndRotation(SpawnLocation, SpawnRotation, false, nullptr, ETeleportType::ResetPhysics);

		// 4. 데이터 로드 (FAIUnitStats로 자동 캐스팅됨)
		FEnemyStats* StatData = StatsDataTable->FindRow<FEnemyStats>(EnemyRowName, TEXT(""));
		FEnemyAssets* AssetData = AssetsDataTable->FindRow<FEnemyAssets>(EnemyRowName, TEXT(""));

		if (StatData && AssetData)
		{
			// 통합된 InitializeUnit 호출
			NewUnit->InitializeUnit(StatData, AssetData);

			// AI 컨트롤러 및 BT 설정
			AAIController* AIC = Cast<AAIController>(NewUnit->GetController());
			if (!AIC)
			{
				NewUnit->SpawnDefaultController();
				AIC = Cast<AAIController>(NewUnit->GetController());
			}

			if (AIC)
			{
				AIC->Possess(NewUnit);
				if (!AssetData->BehaviorTree.IsNull())
				{
					UBehaviorTree* BT = AssetData->BehaviorTree.LoadSynchronous();
					if (BT) AIC->RunBehaviorTree(BT);
				}
			}
			UE_LOG(LogTemp, Log, TEXT("Enemy Spawned: %s"), *EnemyRowName.ToString());
		}
	}

	// 5. [중요] 기존 웨이브 카운팅 및 다음 웨이브 전환 로직
	CurrentSpawnCountInWave++;
	if (CurrentSpawnCountInWave >= WaveConfigs[CurrentWaveIndex].SpawnCount)
	{
		CurrentSpawnCountInWave = 0;
		int32 FinishedIdx = CurrentWaveIndex;
		CurrentWaveIndex++; // 다음 웨이브로

		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);

		// 다음 웨이브가 있다면 타이머 재설정
		if (WaveConfigs.IsValidIndex(CurrentWaveIndex))
		{
			float NextDelay = WaveConfigs[FinishedIdx].NextWaveDelay;
			GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AUnitSpawner::SpawnUnit,
				WaveConfigs[CurrentWaveIndex].SpawnInterval, true, NextDelay);

			UE_LOG(LogTemp, Warning, TEXT("Wave %d Finished. Next Wave in %f s"), FinishedIdx, NextDelay);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("All Waves Finished!"));
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