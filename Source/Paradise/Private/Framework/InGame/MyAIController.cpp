// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/InGame/MyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AI/MonsterAI.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Characters/AIUnit/BaseUnit.h"
#include "Objects/HomeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/Core/ParadiseGameInstance.h"

AMyAIController::AMyAIController()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	if (SightConfig)
	{
		SightConfig->SightRadius = 800.f;
		SightConfig->LoseSightRadius = 1000.f;
		SightConfig->PeripheralVisionAngleDegrees = 90.f;
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

		if (AIPerception)
		{
			AIPerception->ConfigureSense(*SightConfig);
			AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());
		}
	}

	if (AIPerception)
	{
		AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AMyAIController::OnTargetDetected);
	}
}

void AMyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BTAsset && BBAsset)
	{
		UBlackboardComponent* BBRawPtr = Blackboard.Get();
		if (UseBlackboard(BBAsset, BBRawPtr))
		{
			// 유닛에 이미 ID가 부여되어 있다면 즉시 스탯 로드
			if (ABaseUnit* SelfUnit = Cast<ABaseUnit>(InPawn))
			{
				if (!SelfUnit->GetUnitID().IsNone())
				{
					LoadUnitStatsFromTable();
				}
			}
			RunBehaviorTree(BTAsset);
		}
	}
}

void AMyAIController::LoadUnitStatsFromTable()
{
	ABaseUnit* SelfUnit = Cast<ABaseUnit>(GetPawn());
	UParadiseGameInstance* GI = Cast<UParadiseGameInstance>(GetGameInstance());

	if (SelfUnit && GI && Blackboard)
	{
		// GameInstance의 템플릿 함수를 사용하여 데이터 테이블 행 검색
		FEnemyStats* MyStats = GI->GetDataTableRow<FEnemyStats>(GI->EnemyStatsDataTable, SelfUnit->GetUnitID());

		if (MyStats)
		{
			// 블랙보드 키 "TargetAttackRange"에 데이터 테이블의 AttackRange 값을 저장
			Blackboard->SetValueAsFloat(TEXT("TargetAttackRange"), MyStats->AttackRange);
			UE_LOG(LogTemp, Log, TEXT("[%s] Blackboard 'TargetAttackRange' set to: %f"), *SelfUnit->GetName(), MyStats->AttackRange);
		}
	}
}

void AMyAIController::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
	if (Blackboard == nullptr || Actor == nullptr) return;

	if (Actor->IsHidden())
	{
		AActor* CurrentTarget = Cast<AActor>(Blackboard->GetValueAsObject(BB_KEYS::TargetActor));
		if (CurrentTarget == Actor)
		{
			Blackboard->ClearValue(BB_KEYS::TargetActor);
		}
		return;
	}

	AActor* CurrentTarget = Cast<AActor>(Blackboard->GetValueAsObject(BB_KEYS::TargetActor));
	if (CurrentTarget && CurrentTarget->IsValidLowLevel())
	{
		if (CurrentTarget == Actor && !Stimulus.WasSuccessfullySensed())
		{
			Blackboard->ClearValue(BB_KEYS::TargetActor);
		}
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		ABaseUnit* TargetUnit = Cast<ABaseUnit>(Actor);
		ABaseUnit* SelfUnit = Cast<ABaseUnit>(GetPawn());

		if (TargetUnit && SelfUnit && SelfUnit->IsEnemy(TargetUnit))
		{
			Blackboard->SetValueAsObject(BB_KEYS::TargetActor, Actor);
		}
	}
}