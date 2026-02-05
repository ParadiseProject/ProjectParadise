// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "MyAIController.generated.h"

UCLASS()
class PARADISE_API AMyAIController : public AAIController
{
    GENERATED_BODY()

public:
    AMyAIController();

protected:
    virtual void OnPossess(APawn* InPawn) override;

private:
    /** @brief 에디터의 디테일 패널에서 비헤이비어 트리 에셋을 선택할 수 있게 합니다. */
    UPROPERTY(EditAnywhere, Category = "AI")
    class UBehaviorTree* BTAsset;

    /** @brief 에디터의 디테일 패널에서 블랙보드 에셋을 선택할 수 있게 합니다. */
    UPROPERTY(EditAnywhere, Category = "AI")
    class UBlackboardData* BBAsset;

    /** @brief 감지 시스템 컴포넌트 */
    UPROPERTY(VisibleAnywhere, Category = "AI")
    class UAIPerceptionComponent* AIPerception;

    /** @brief 시각 감지 설정 */
    class UAISenseConfig_Sight* SightConfig;

    /** @brief 감지 업데이트 시 호출될 함수 */
    UFUNCTION()
    void OnTargetDetected(AActor* Actor, FAIStimulus Stimulus);
};