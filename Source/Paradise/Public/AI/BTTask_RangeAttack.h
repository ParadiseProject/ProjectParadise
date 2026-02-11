// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_RangeAttack.generated.h"

UCLASS()
class PARADISE_API UBTTask_RangeAttack : public UBTTask_BlackboardBase
{
    GENERATED_BODY()

public:
    UBTTask_RangeAttack();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    // 원거리 공격 사거리
    UPROPERTY(EditAnywhere, Category = "Combat")
    float AttackRange = 800.0f;
};