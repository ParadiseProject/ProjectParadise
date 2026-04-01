#include "AI/Unit/BTService_CalcLaneTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Characters/Base/CharacterBase.h"
#include "Objects/HomeBase.h"

UBTService_CalcLaneTarget::UBTService_CalcLaneTarget()
{
	NodeName = TEXT("Calculate Lane Target");
	bNotifyTick = true;
	Interval = 0.2f; // 0.2초 주기로 갱신
}

void UBTService_CalcLaneTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!BBComp || !AICon || !AICon->GetPawn()) return;

	AICon->ClearFocus(EAIFocusPriority::Gameplay);
	FVector MyLoc = AICon->GetPawn()->GetActorLocation();
	float AssignedLaneY = BBComp->GetValueAsFloat(AssignedLaneYKey.SelectedKeyName);

	// 1. MyAIController의 AIPerception이 세팅한 타겟 가져오기
	AActor* CurrentTarget = Cast<AActor>(BBComp->GetValueAsObject(TargetActorKey.SelectedKeyName));

	// 2. 타겟이 없거나, 죽었다면 -> 적 기지로 타겟 변경
	if (!IsValid(CurrentTarget) || (Cast<ACharacterBase>(CurrentTarget) && Cast<ACharacterBase>(CurrentTarget)->IsDead()))
	{
		CurrentTarget = Cast<AActor>(BBComp->GetValueAsObject(EnemyBaseKey.SelectedKeyName));

		// 죽은 타겟이 블랙보드에 남아있다면 비워줌 (AIPerception이 새 적을 찾을 수 있게)
		if (BBComp->GetValueAsObject(TargetActorKey.SelectedKeyName))
		{
			BBComp->ClearValue(TargetActorKey.SelectedKeyName);
		}

	}

	// 3. 목표 좌표 계산
	FVector MoveTarget;
	if (CurrentTarget)
	{
		if (Cast<AHomeBase>(CurrentTarget))
		{
			// 기지는 거대하므로 기존처럼 레인 유지하며 직진
			MoveTarget = FVector(CurrentTarget->GetActorLocation().X, AssignedLaneY, MyLoc.Z);
		}
		else
		{
			// =========================================================
			// 🚨 [수정] 겹침 방지: 타겟의 정중앙이 아닌 사거리 끝부분으로 이동
			// =========================================================
			FVector TargetLoc = CurrentTarget->GetActorLocation();

			// 1. 타겟에서 내 유닛을 향하는 방향(Direction) 벡터를 구함
			FVector DirToMe = (MyLoc - TargetLoc).GetSafeNormal();

			// 2. 타겟 중심으로부터 어느 정도 떨어져서 멈출 것인가? (내 사거리의 80% 지점)
			// 100%로 잡으면 오차 때문에 타격이 안 될 수 있으므로 80%~90%가 적당합니다.
			float StopDistance = AICon->GetPawn()->GetSimpleCollisionRadius() + CurrentTarget->GetSimpleCollisionRadius();
			AUnitBase* MyUnit = Cast<AUnitBase>(AICon->GetPawn());
			if (MyUnit)
			{
				StopDistance += (MyUnit->GetAttackRange() * 0.8f);
			}

			// 3. 타겟 위치에서 내 방향으로 StopDistance만큼 당겨온 좌표가 최종 목적지!
			MoveTarget = TargetLoc + (DirToMe * StopDistance);
		}
	}
	else
	{
		MoveTarget = MyLoc;
	}

	// 기존처럼 블랙보드 갱신하기 전에, 의미 있는 거리 변화가 있을 때만 갱신 (경련 방지)
	FVector OldMoveTarget = BBComp->GetValueAsVector(MoveDestinationKey.SelectedKeyName);
	if (FVector::Dist(OldMoveTarget, MoveTarget) > 50.0f)
	{
		BBComp->SetValueAsVector(MoveDestinationKey.SelectedKeyName, MoveTarget);
	}
}