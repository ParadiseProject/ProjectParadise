// Fill out your copyright notice in the Description page of Project Settings.

#include "Objects/UnitSpawner.h"
#include "Characters/AIUnit/BaseUnit.h"
#include "Framework/Core/ParadiseGameInstance.h"
#include "Framework/System/ObjectPoolSubsystem.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "NavigationSystem.h"
#include "BrainComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AUnitSpawner::AUnitSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AUnitSpawner::BeginPlay()
{
	Super::BeginPlay();

	UObjectPoolSubsystem* PoolSubsystem = GetWorld()->GetSubsystem<UObjectPoolSubsystem>();
	if (!PoolSubsystem) return;

	// 프리풀링
	for (int32 i = 0; i < PreSpawnCount; i++)
	{
		ABaseUnit* TempUnit = PoolSubsystem->SpawnPoolActor<ABaseUnit>(UnitClass, GetActorLocation(), GetActorRotation(), this, nullptr);
		if (TempUnit) PoolSubsystem->ReturnToPool(TempUnit);
	}

	// 첫 번째 웨이브 시작
	if (WaveConfigs.IsValidIndex(0))
	{
		float InitialDelay = 1.0f;
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AUnitSpawner::SpawnUnit, WaveConfigs[0].SpawnInterval, true, InitialDelay);
	}
}

void AUnitSpawner::SpawnUnit()
{
	// 💡 [핵심] 현재 웨이브에 맞는 이름을 기존 EnemyRowName 변수에 대입
	if (WaveConfigs.IsValidIndex(CurrentWaveIndex))
	{
		EnemyRowName = WaveConfigs[CurrentWaveIndex].UnitRowName;
	}
	else
	{
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
		UE_LOG(LogTemp, Log, TEXT("All Waves Completed."));
		return;
	}

	UParadiseGameInstance* GI = Cast<UParadiseGameInstance>(GetGameInstance());
	UObjectPoolSubsystem* PoolSubsystem = GetWorld()->GetSubsystem<UObjectPoolSubsystem>();

	// 유효성 검사 (EnemyRowName이 위에서 대입되었으므로 기존 로직 유지)
	if (!GI || !PoolSubsystem || !StatsDataTable || !AssetsDataTable || EnemyRowName.IsNone()) return;

	FVector FootLocation = GetRandomSpawnLocation();
	FVector SpawnLocation = FootLocation + FVector(0.f, 0.f, 2.0f);
	FRotator SpawnRotation = FRotator(0.f, FMath::RandRange(0.f, 360.f), 0.f);

	ABaseUnit* NewUnit = PoolSubsystem->SpawnPoolActor<ABaseUnit>(UnitClass, SpawnLocation, SpawnRotation, this, nullptr);

	if (NewUnit)
	{
		NewUnit->SetActorLocationAndRotation(SpawnLocation, SpawnRotation, false, nullptr, ETeleportType::ResetPhysics);

		// 💡 기존 로직 그대로: EnemyRowName을 사용하여 데이터 테이블에서 행을 찾음
		FEnemyStats* StatData = StatsDataTable->FindRow<FEnemyStats>(EnemyRowName, TEXT(""));
		FEnemyAssets* AssetData = AssetsDataTable->FindRow<FEnemyAssets>(EnemyRowName, TEXT(""));

		NewUnit->InitializeUnit(StatData, AssetData);

		if (AssetData)
		{
			AAIController* AIC = Cast<AAIController>(NewUnit->GetController());
			if (!AIC)
			{
				NewUnit->SpawnDefaultController();
				AIC = Cast<AAIController>(NewUnit->GetController());
			}

			if (AIC)
			{
				AIC->Possess(NewUnit);

				FTimerHandle AIStartTimer;
				GetWorldTimerManager().SetTimer(AIStartTimer, [AIC, AssetData]()
					{
						if (AIC && !AssetData->BehaviorTree.IsNull())
						{
							UBehaviorTree* BT = AssetData->BehaviorTree.LoadSynchronous();
							if (BT)
							{
								AIC->RunBehaviorTree(BT);
								if (AIC->GetBrainComponent()) AIC->GetBrainComponent()->RestartLogic();
							}
						}
					}, 0.1f, false);
			}
		}
	}

	// 💡 웨이브 진행 카운트 및 전환 로직
	CurrentSpawnCountInWave++;

	if (CurrentSpawnCountInWave >= WaveConfigs[CurrentWaveIndex].SpawnCount)
	{
		CurrentSpawnCountInWave = 0;
		int32 PrevIndex = CurrentWaveIndex;
		CurrentWaveIndex++;

		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);

		if (WaveConfigs.IsValidIndex(CurrentWaveIndex))
		{
			float NextDelay = WaveConfigs[PrevIndex].NextWaveDelay;
			float NextInterval = WaveConfigs[CurrentWaveIndex].SpawnInterval;

			GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AUnitSpawner::SpawnUnit, NextInterval, true, NextDelay);
			UE_LOG(LogTemp, Warning, TEXT("Wave %d Finished. Next Wave in %f seconds."), PrevIndex, NextDelay);
		}
	}
}

FVector AUnitSpawner::GetRandomSpawnLocation()
{
	FVector Origin = GetActorLocation();
	float RandomX = FMath::RandRange(-SpawnExtent.X, SpawnExtent.X);
	float RandomY = FMath::RandRange(-SpawnExtent.Y, SpawnExtent.Y);
	FVector TargetPoint = Origin + FVector(RandomX, RandomY, 0.0f);

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