// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/BTTask_Attack.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

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
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!AIController || !BB) return EBTNodeResult::Failed;

    APawn* ControllingPawn = AIController->GetPawn();

    AActor* Target = Cast<AActor>(BB->GetValueAsObject(TEXT("TargetActor")));
    if (!Target)
    {
        Target = Cast<AActor>(BB->GetValueAsObject(TEXT("HomeBaseActor")));
    }

    if (Target)
    {
        float DamageAmount = 20.0f;
        UGameplayStatics::ApplyDamage(Target, DamageAmount, AIController, ControllingPawn, nullptr);

        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}