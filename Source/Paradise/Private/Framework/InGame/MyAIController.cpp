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

    UBlackboardComponent* BBComp = Blackboard.Get();
    if (BTAsset && BBAsset && UseBlackboard(BBAsset, BBComp))
    {
        Blackboard = BBComp;
        ABaseUnit* SelfUnit = Cast<ABaseUnit>(InPawn);
        UParadiseGameInstance* GI = Cast<UParadiseGameInstance>(GetGameInstance());

        if (SelfUnit && GI)
        {
            // 1. 사거리 데이터 로드
            FEnemyStats* MyStats = GI->GetDataTableRow<FEnemyStats>(GI->EnemyStatsDataTable, SelfUnit->GetUnitID());
            if (MyStats)
            {
                Blackboard->SetValueAsFloat(TEXT("TargetAttackRange"), MyStats->AttackRange);
            }

            // 2. 적대적인 기지만 찾아내기
            TArray<AActor*> FoundBases;
            UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHomeBase::StaticClass(), FoundBases);

            for (AActor* Actor : FoundBases)
            {
                AHomeBase* HomeBase = Cast<AHomeBase>(Actor);
                if (HomeBase)
                {
                    if (SelfUnit->IsEnemy(HomeBase))
                    {
                        Blackboard->SetValueAsObject(TEXT("HomeBaseActor"), HomeBase);
                        UE_LOG(LogTemp, Log, TEXT("🚀 [%s] Target Base Found: %s"), *SelfUnit->GetName(), *HomeBase->GetName());
                        break;
                    }
                }
            }
        }
        RunBehaviorTree(BTAsset);
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

    if (Stimulus.WasSuccessfullySensed())
    {
        ABaseUnit* TargetUnit = Cast<ABaseUnit>(Actor);
        ABaseUnit* SelfUnit = Cast<ABaseUnit>(GetPawn());

        if (TargetUnit && SelfUnit && SelfUnit->IsEnemy(TargetUnit))
        {
            Blackboard->SetValueAsObject(BB_KEYS::TargetActor, Actor);
        }
    }
    else
    {
        // 시야에서 사라졌을 때 타겟 클리어
        AActor* CurrentTarget = Cast<AActor>(Blackboard->GetValueAsObject(BB_KEYS::TargetActor));
        if (CurrentTarget == Actor)
        {
            Blackboard->ClearValue(BB_KEYS::TargetActor);
        }
    }
}