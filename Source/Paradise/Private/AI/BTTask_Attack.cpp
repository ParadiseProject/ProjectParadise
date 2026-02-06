// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/BTTask_Attack.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/AIUnit/BaseUnit.h"

UBTTask_Attack::UBTTask_Attack()
{
	NodeName = TEXT("Attack Target");
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!AIController || !BB) return EBTNodeResult::Failed;

	// 내 유닛 정보
	ABaseUnit* MyUnit = Cast<ABaseUnit>(AIController->GetPawn());

	// 타겟 액터 확보
	AActor* Target = Cast<AActor>(BB->GetValueAsObject(TEXT("TargetActor")));
	if (!Target)
	{
		Target = Cast<AActor>(BB->GetValueAsObject(TEXT("HomeBaseActor")));
	}

	// 타겟 유닛 정보
	ABaseUnit* TargetUnit = Cast<ABaseUnit>(Target);

	// 타겟이 존재하고, 나와 적 관계일 때만 데미지 적용
	if (MyUnit && TargetUnit && MyUnit->IsEnemy(TargetUnit))
	{
		float DamageAmount = 20.0f;
		UGameplayStatics::ApplyDamage(TargetUnit, DamageAmount, AIController, MyUnit, nullptr);

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}