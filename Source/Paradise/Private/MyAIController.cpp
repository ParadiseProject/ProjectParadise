// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "MonsterAI.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

AMyAIController::AMyAIController()
{
    // 1. 퍼셉션 컴포넌트 생성
    AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

    // 2. 시야 설정
    SightConfig->SightRadius = 2000.f;          // 감지 거리
    SightConfig->LoseSightRadius = 2500.f;      // 놓치는 거리
    SightConfig->PeripheralVisionAngleDegrees = 90.f; // 시야각
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

    // 3. 컴포넌트에 설정 적용
    AIPerception->ConfigureSense(*SightConfig);
    AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());

    // 4. 감지 이벤트 연결
    AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AMyAIController::OnTargetDetected);
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

void AMyAIController::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
    if (Blackboard == nullptr)
    {
        return;
    }

    if (Stimulus.WasSuccessfullySensed())
    {
        Blackboard->SetValueAsObject(BB_KEYS::TargetActor, Actor);
        Blackboard->SetValueAsEnum(BB_KEYS::AIState, static_cast<uint8>(EMonsterState::Pursuit));
        UE_LOG(LogTemp, Warning, TEXT("Target Detected: %s"), *Actor->GetName());
    }
    else
    {
        Blackboard->SetValueAsObject(BB_KEYS::TargetActor, nullptr);
        Blackboard->SetValueAsEnum(BB_KEYS::AIState, static_cast<uint8>(EMonsterState::Idle));
        UE_LOG(LogTemp, Warning, TEXT("Target Lost"));
    }
}