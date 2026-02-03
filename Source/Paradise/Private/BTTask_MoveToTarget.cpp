// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveToTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MonsterAI.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_MoveToTarget::UBTTask_MoveToTarget()
{
    NodeName = TEXT("Move To Target Location");
}

EBTNodeResult::Type UBTTask_MoveToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();

    if (AIController && BBComp)
    {
        FVector TargetLoc = BBComp->GetValueAsVector(BB_KEYS::TargetLocation);

        if (AIController->MoveToLocation(TargetLoc) == EPathFollowingRequestResult::Failed)
        {
            return EBTNodeResult::Failed;
        }

        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}