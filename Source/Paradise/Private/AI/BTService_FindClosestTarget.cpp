// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/BTService_FindClosestTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Characters/AIUnit/BaseUnit.h"
#include "EngineUtils.h"

UBTService_FindClosestTarget::UBTService_FindClosestTarget()
{
	NodeName = "Find Closest Target";
	Interval = 0.2f; // 0.2초마다 탐색
}

void UBTService_FindClosestTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	ABaseUnit* SelfUnit = Cast<ABaseUnit>(ControllingPawn);
	if (!SelfUnit) return;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;

	// 현재 타겟이 살아있는지 먼저 확인
	AActor* CurrentTarget = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	ABaseUnit* TargetUnit = Cast<ABaseUnit>(CurrentTarget);

	// 이미 타겟이 있고, 그 타겟이 살아있다면 타겟을 바꾸지 않고 유지함
	if (TargetUnit && !TargetUnit->bIsDead)
	{
		return;
	}

	// 타겟이 없거나 죽었다면 새로운 적을 찾음
	ABaseUnit* ClosestEnemy = nullptr;
	float MinDistance = SearchRadius;

	for (TActorIterator<ABaseUnit> It(GetWorld()); It; ++It)
	{
		ABaseUnit* OtherUnit = *It;
		if (OtherUnit != SelfUnit && !OtherUnit->bIsDead && SelfUnit->IsEnemy(OtherUnit))
		{
			float Distance = FVector::Dist(SelfUnit->GetActorLocation(), OtherUnit->GetActorLocation());
			if (Distance < MinDistance)
			{
				MinDistance = Distance;
				ClosestEnemy = OtherUnit;
			}
		}
	}

	BB->SetValueAsObject(TargetActorKey.SelectedKeyName, ClosestEnemy);
}