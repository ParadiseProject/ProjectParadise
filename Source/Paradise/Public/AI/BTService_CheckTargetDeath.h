// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CheckTargetDeath.generated.h"

UCLASS()
class PARADISE_API UBTService_CheckTargetDeath : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_CheckTargetDeath();

protected:
	// 서비스가 주기적으로 실행할 로직
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;
};