// Fill out your copyright notice in the Description page of Project Settings.

#include "Objects/FamiliarSpawner.h"
#include "Characters/AIUnit/BaseUnit.h"
#include "Framework/System/ObjectPoolSubsystem.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BrainComponent.h"
#include "Data/Structs/UnitStructs.h"

void AFamiliarSpawner::SpawnFamiliarBySlot(int32 SlotIndex)
{
	if (!FamiliarSlots.IsValidIndex(SlotIndex)) return;

	FName RowName = FamiliarSlots[SlotIndex];
	UObjectPoolSubsystem* PoolSubsystem = GetWorld()->GetSubsystem<UObjectPoolSubsystem>();

	if (!PoolSubsystem || !UnitClass || !StatsDataTable || !AssetsDataTable) return;

	FVector SpawnLocation = GetRandomSpawnLocation() + FVector(0.f, 0.f, 100.0f);
	FRotator SpawnRotation = GetActorRotation();

	ABaseUnit* NewUnit = PoolSubsystem->SpawnPoolActor<ABaseUnit>(UnitClass, SpawnLocation, SpawnRotation, this, nullptr);

	if (NewUnit)
	{
		NewUnit->SetActorLocationAndRotation(SpawnLocation, SpawnRotation, false, nullptr, ETeleportType::ResetPhysics);

		// 아군 전용 구조체로 데이터 테이블 검색
		FFamiliarStats* StatData = StatsDataTable->FindRow<FFamiliarStats>(RowName, TEXT(""));
		FFamiliarAssets* AssetData = AssetsDataTable->FindRow<FFamiliarAssets>(RowName, TEXT(""));

		if (StatData && AssetData)
		{
			// FFamiliarStats 포인터는 InitializeUnit의 인자인 FAIUnitStats 포인터로 자동 형변환됩니다.
			NewUnit->InitializeUnit(StatData, AssetData);

			AAIController* AIC = Cast<AAIController>(NewUnit->GetController());
			if (!AIC) { NewUnit->SpawnDefaultController(); AIC = Cast<AAIController>(NewUnit->GetController()); }

			if (AIC)
			{
				AIC->Possess(NewUnit);
				if (!AssetData->BehaviorTree.IsNull())
				{
					UBehaviorTree* BT = AssetData->BehaviorTree.LoadSynchronous();
					if (BT)
					{
						AIC->RunBehaviorTree(BT);
						if (AIC->GetBrainComponent()) AIC->GetBrainComponent()->RestartLogic();
					}
				}
			}
			UE_LOG(LogTemp, Warning, TEXT("Familiar Success: %s"), *RowName.ToString());
		}
	}
}