// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
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
};