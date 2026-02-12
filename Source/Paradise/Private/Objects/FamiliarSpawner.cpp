// Fill out your copyright notice in the Description page of Project Settings.

#include "Objects/FamiliarSpawner.h"
#include "Characters/AIUnit/BaseUnit.h"
#include "Framework/System/ObjectPoolSubsystem.h"
#include "Framework/InGame/MyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BrainComponent.h"
#include "Data/Structs/UnitStructs.h"

void AFamiliarSpawner::SpawnFamiliarByID(FName UnitID)
{
	// 1. 유효성 검사
	if (UnitID.IsNone())
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnFamiliarByID: UnitID is None!"));
		return;
	}

	UObjectPoolSubsystem* PoolSubsystem = GetWorld()->GetSubsystem<UObjectPoolSubsystem>();
	if (!PoolSubsystem || !UnitClass || !StatsDataTable || !AssetsDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnFamiliarByID: Essential assets or subsystem missing!"));
		return;
	}

	// 2. 스폰 위치 및 회전 설정
	FVector SpawnLocation = GetRandomSpawnLocation() + FVector(0.f, 0.f, 100.0f);
	FRotator SpawnRotation = GetActorRotation();

	// 3. 오브젝트 풀링을 통한 스폰
	ABaseUnit* NewUnit = PoolSubsystem->SpawnPoolActor<ABaseUnit>(UnitClass, SpawnLocation, SpawnRotation, this, nullptr);

	if (NewUnit)
	{
		// 풀링 유닛 위치 초기화
		NewUnit->SetActorLocationAndRotation(SpawnLocation, SpawnRotation, false, nullptr, ETeleportType::ResetPhysics);

		// 유닛에게 ID 부여
		NewUnit->SetUnitID(UnitID);

		// 4. 에셋 데이터 테이블에서 외형 및 BT 정보 로드
		FFamiliarStats* StatData = StatsDataTable->FindRow<FFamiliarStats>(UnitID, TEXT(""));
		FFamiliarAssets* AssetData = AssetsDataTable->FindRow<FFamiliarAssets>(UnitID, TEXT(""));

		if (StatData && AssetData)
		{
			// 유닛 외형 및 기본 스탯 초기화
			NewUnit->InitializeUnit(StatData, AssetData);

			// 5. AI 컨트롤러 설정
			AMyAIController* AIC = Cast<AMyAIController>(NewUnit->GetController());
			if (!AIC)
			{
				NewUnit->SpawnDefaultController();
				AIC = Cast<AMyAIController>(NewUnit->GetController());
			}

			if (AIC)
			{
				// 빙의 수행 -> MyAIController::OnPossess()가 실행되며 GI 테이블의 사거리를 가져옴
				AIC->Possess(NewUnit);

				// 6. 비헤이비어 트리 실행
				if (!AssetData->BehaviorTree.IsNull())
				{
					UBehaviorTree* BT = AssetData->BehaviorTree.LoadSynchronous();
					if (BT)
					{
						AIC->RunBehaviorTree(BT);
						// 필요한 경우 로직 재시작
						if (AIC->GetBrainComponent()) AIC->GetBrainComponent()->RestartLogic();
					}
				}
			}
			UE_LOG(LogTemp, Warning, TEXT("Successfully Spawned Familiar: %s"), *UnitID.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to find data for UnitID: %s in DataTables"), *UnitID.ToString());
		}
	}
}