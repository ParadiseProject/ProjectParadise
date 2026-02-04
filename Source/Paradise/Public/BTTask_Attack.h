// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Attack.generated.h"

/**
 * @class UBTTask_Attack
 * @brief 몬스터가 타겟에게 도달했을 때 공격 로직(로그 출력 등)을 수행하는 비헤이비어 트리 태스크 클래스입니다.
 */
UCLASS()
class PARADISE_API UBTTask_Attack : public UBTTaskNode
{
    GENERATED_BODY()

public:
    /**
     * @brief UBTTask_Attack 생성자. 태스크의 이름을 설정합니다.
     */
    UBTTask_Attack();

    /**
     * @brief 태스크가 실행될 때 호출되는 핵심 함수입니다.
     * @param OwnerComp 이 태스크를 실행하는 비헤이비어 트리 컴포넌트 참조
     * @param NodeMemory 노드에서 사용하는 메모리 블록
     * @return EBTNodeResult::Type 태스크의 성공, 실패, 또는 진행 중 상태를 반환합니다.
     */
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};