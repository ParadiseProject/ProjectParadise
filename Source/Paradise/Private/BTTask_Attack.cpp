// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_Attack.h"
#include "AIController.h"

/**
 * @brief 생성자: 노드의 이름을 "Attack Target"으로 초기화합니다.
 */
UBTTask_Attack::UBTTask_Attack()
{
    NodeName = TEXT("Attack Target");
}

/**
 * @brief 공격 로직을 실행합니다. 현재는 콘솔 창에 공격 로그를 출력하는 기능을 수행합니다.
 */
EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();

    if (AIController)
    {
        APawn* ControlledPawn = AIController->GetPawn();
        FString MonsterName = ControlledPawn ? ControlledPawn->GetName() : TEXT("Unknown Monster");

        UE_LOG(LogTemp, Warning, TEXT("[%s] 타겟을 공격했습니다! (Damage: 10)"), *MonsterName);

        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}