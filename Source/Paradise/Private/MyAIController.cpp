// Fill out your copyright notice in the Description page of Project Settings.

#include "MyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "MonsterAI.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BaseUnit.h"
#include "HomeBase.h"
#include "Kismet/GameplayStatics.h"

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
            ABaseUnit* SelfUnit = Cast<ABaseUnit>(InPawn);

            TArray<AActor*> FoundBases;
            UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHomeBase::StaticClass(), FoundBases);

            UE_LOG(LogTemp, Warning, TEXT("[%s] Found %d Bases in World"), *InPawn->GetName(), FoundBases.Num());

            for (AActor* Actor : FoundBases)
            {
                AHomeBase* HomeBase = Cast<AHomeBase>(Actor);
                if (HomeBase && SelfUnit && HomeBase->TeamID != SelfUnit->TeamID && Actor->ActorHasTag(TEXT("Base")))
                {
                    Blackboard->SetValueAsObject(TEXT("HomeBaseActor"), Actor);
                    UE_LOG(LogTemp, Warning, TEXT("[%s] Target Base Set: %s"), *InPawn->GetName(), *Actor->GetName());
                    break;
                }
            }

            RunBehaviorTree(BTAsset);
        }
    }
}

void AMyAIController::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
    if (Blackboard == nullptr) return;

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

        if (TargetUnit && SelfUnit && TargetUnit->TeamID != SelfUnit->TeamID)
        {
            Blackboard->SetValueAsObject(BB_KEYS::TargetActor, Actor);
        }
    }
}