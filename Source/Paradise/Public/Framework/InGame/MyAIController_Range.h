// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MyAIController_Range.generated.h"

UCLASS()
class PARADISE_API AMyAIController_Range : public AAIController
{
	GENERATED_BODY()

protected:
	virtual void OnPossess(APawn* InPawn) override;

public:
	UPROPERTY(EditAnywhere, Category = "AI")
	class UBehaviorTree* BTAsset;
};