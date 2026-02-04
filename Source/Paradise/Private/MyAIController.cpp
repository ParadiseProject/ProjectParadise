// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "MonsterAI.h"

AMyAIController::AMyAIController()
{
}

/**
 * @brief 몬스터(Pawn)가 스폰되어 컨트롤러가 빙의(Possess)될 때 실행됩니다.
 */
void AMyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (BTAsset && BBAsset)
    {
        UBlackboardComponent* BBComp = Blackboard;

        if (UseBlackboard(BBAsset, BBComp))
        {
            // 1. 초기 목적지 설정 (예: X=2000 지점의 적진)
            Blackboard->SetValueAsVector(BB_KEYS::TargetLocation, FVector(2000.f, 0.f, 0.f));

            // 2. 초기 상태를 Idle(대기)로 설정
            Blackboard->SetValueAsEnum(BB_KEYS::AIState, static_cast<uint8>(EMonsterState::Idle));

            // 3. 결정된 데이터를 바탕으로 비헤이비어 트리 가동
            RunBehaviorTree(BTAsset);
        }
    }
}