// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_MoveToTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"
// #include "MonsterAI.h"

UBTTask_MoveToTarget::UBTTask_MoveToTarget()
{
    NodeName = TEXT("Move To Target Location");

    TargetLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_MoveToTarget, TargetLocationKey));
}

EBTNodeResult::Type UBTTask_MoveToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();

    if (AIController && BBComp)
    {
        FVector TargetLoc = BBComp->GetValueAsVector(TargetLocationKey.SelectedKeyName);

        if (AIController->MoveToLocation(TargetLoc) == EPathFollowingRequestResult::Failed)
        {
            return EBTNodeResult::Failed;
        }

        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}