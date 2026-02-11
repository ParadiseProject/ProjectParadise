// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/InGame/MyAIController_Range.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

void AMyAIController_Range::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BTAsset)
	{
		RunBehaviorTree(BTAsset);
	}
}