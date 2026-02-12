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

	if (!PoolSubsystem || !UnitClass || EnemyRowName.IsNone()) return;

	FVector SpawnLocation = GetRandomSpawnLocation() + FVector(0.f, 0.f, 100.0f);
	FRotator SpawnRotation = FRotator(0.f, FMath::RandRange(0.f, 360.f), 0.f);

	ABaseUnit* NewUnit = PoolSubsystem->SpawnPoolActor<ABaseUnit>(UnitClass, SpawnLocation, SpawnRotation, this, nullptr);

	if (NewUnit)
	{
		NewUnit->SetActorLocationAndRotation(SpawnLocation, SpawnRotation, false, nullptr, ETeleportType::ResetPhysics);

		// 1. 유닛에게 ID 부여
		NewUnit->SetUnitID(EnemyRowName);

		// 2. 데이터 테이블에서 에셋 정보 가져오기
		FEnemyStats* StatData = StatsDataTable->FindRow<FEnemyStats>(EnemyRowName, TEXT(""));
		FEnemyAssets* AssetData = AssetsDataTable->FindRow<FEnemyAssets>(EnemyRowName, TEXT(""));

		if (StatData && AssetData)
		{
			// 유닛 외형 및 기본 스탯 초기화
			NewUnit->InitializeUnit(StatData, AssetData);

			// 3. AI 컨트롤러 설정
			AMyAIController* AIC = Cast<AMyAIController>(NewUnit->GetController());
			if (!AIC)
			{
				NewUnit->SpawnDefaultController();
				AIC = Cast<AMyAIController>(NewUnit->GetController());
			}

			if (AIC)
			{
				// AIC의 OnPossess가 호출되며 GI에서 데이터를 가져옴
				AIC->Possess(NewUnit);

				// 데이터 테이블에 등록된 BT 실행
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

	// 웨이브 관리 로직
	CurrentSpawnCountInWave++;
	if (CurrentSpawnCountInWave >= WaveConfigs[CurrentWaveIndex].SpawnCount)
	{
		CurrentSpawnCountInWave = 0;
		int32 FinishedIdx = CurrentWaveIndex;
		CurrentWaveIndex++;

		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);

		if (WaveConfigs.IsValidIndex(CurrentWaveIndex))
		{
			GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AUnitSpawner::SpawnUnit,
				WaveConfigs[CurrentWaveIndex].SpawnInterval, true, WaveConfigs[FinishedIdx].NextWaveDelay);
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