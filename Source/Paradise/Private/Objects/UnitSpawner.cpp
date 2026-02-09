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

	for (int32 i = 0; i < PreSpawnCount; i++)
	{
		ABaseUnit* TempUnit = PoolSubsystem->SpawnPoolActor<ABaseUnit>(UnitClass, GetActorLocation(), GetActorRotation(), this, nullptr);
		if (TempUnit) PoolSubsystem->ReturnToPool(TempUnit);
	}

	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AUnitSpawner::SpawnUnit, SpawnInterval, true, 1.0f);
}

void AUnitSpawner::SpawnUnit()
{
	UParadiseGameInstance* GI = Cast<UParadiseGameInstance>(GetGameInstance());
	UObjectPoolSubsystem* PoolSubsystem = GetWorld()->GetSubsystem<UObjectPoolSubsystem>();

	// 유효성 검사: GI, 풀, 테이블, 행 이름 확인
	if (!GI || !PoolSubsystem || !StatsDataTable || !AssetsDataTable || EnemyRowName.IsNone()) return;

	FVector FootLocation = GetRandomSpawnLocation();
	FVector SpawnLocation = FootLocation + FVector(0.f, 0.f, 2.0f);
	FRotator SpawnRotation = FRotator(0.f, FMath::RandRange(0.f, 360.f), 0.f);

	ABaseUnit* NewUnit = PoolSubsystem->SpawnPoolActor<ABaseUnit>(UnitClass, SpawnLocation, SpawnRotation, this, nullptr);

	if (NewUnit)
	{
		NewUnit->SetActorLocationAndRotation(SpawnLocation, SpawnRotation, false, nullptr, ETeleportType::ResetPhysics);

		// 설정된 데이터 테이블에서 데이터 찾기
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
				// 요청하신 대로 Possess 코드 유지
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