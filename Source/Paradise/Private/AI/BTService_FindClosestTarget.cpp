// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/BTService_FindClosestTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Characters/Base/CharacterBase.h"
#include "Characters/AIUnit/UnitBase.h"
#include "Objects/HomeBase.h"
#include "EngineUtils.h"
#include "Components/CapsuleComponent.h"

UBTService_FindClosestTarget::UBTService_FindClosestTarget()
{
	NodeName = "Find Closest Target";
	Interval = 0.2f;
	SearchRadius = 1500.0f;
}

void UBTService_FindClosestTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!BB || !AIC) return;

	AUnitBase* SelfUnit = Cast<AUnitBase>(AIC->GetPawn());
	if (!SelfUnit) return;

	UObject* RawTarget = BB->GetValueAsObject(TargetActorKey.SelectedKeyName);
	ACharacterBase* CurrentTarget = Cast<ACharacterBase>(RawTarget);

	ACharacterBase* ClosestEnemy = nullptr;
	float MinDistance = SearchRadius;
	float PenetrationMargin = -125.0f;

	// [1단계] 현재 타겟 거리 체크 로그
	if (IsValid(CurrentTarget) && !CurrentTarget->IsDead())
	{
		FBox TargetBox = CurrentTarget->GetComponentsBoundingBox(true);
		FVector ClosestPointOnTarget = TargetBox.GetClosestPointTo(SelfUnit->GetActorLocation());

		float SurfaceDist = FVector::Dist2D(SelfUnit->GetActorLocation(), ClosestPointOnTarget);
		SurfaceDist -= (SelfUnit->GetCapsuleComponent()->GetScaledCapsuleRadius() + PenetrationMargin);
		SurfaceDist = FMath::Max(0.0f, SurfaceDist);

		// 🔍 로그: 기존 타겟과의 거리 출력
		UE_LOG(LogTemp, Log, TEXT("[BT_Service] 현재 타겟(%s)과의 표면 거리: %.2f"), *CurrentTarget->GetName(), SurfaceDist);

		if (SurfaceDist < SearchRadius)
		{
			MinDistance = SurfaceDist;
			ClosestEnemy = CurrentTarget;
		}
	}

	// [2단계] 주변 적 탐색 로그
	for (TActorIterator<ACharacterBase> It(GetWorld()); It; ++It)
	{
		ACharacterBase* OtherChar = *It;
		if (!IsValid(OtherChar) || OtherChar == SelfUnit || OtherChar->IsDead()) continue;
		if (OtherChar == ClosestEnemy) continue;

		if (SelfUnit->IsHostile(OtherChar))
		{
			FBox TargetBox = OtherChar->GetComponentsBoundingBox(true);
			FVector ClosestPointOnTarget = TargetBox.GetClosestPointTo(SelfUnit->GetActorLocation());

			float SurfaceDistance = FVector::Dist2D(SelfUnit->GetActorLocation(), ClosestPointOnTarget);
			SurfaceDistance -= (SelfUnit->GetCapsuleComponent()->GetScaledCapsuleRadius() + PenetrationMargin);
			SurfaceDistance = FMath::Max(0.0f, SurfaceDistance);

			float SwitchThreshold = (ClosestEnemy == CurrentTarget && CurrentTarget != nullptr) ? (MinDistance - 50.0f) : MinDistance;

			if (SurfaceDistance < SwitchThreshold)
			{
				// 🔍 로그: 새로운 더 가까운 적 발견 시 출력
				UE_LOG(LogTemp, Warning, TEXT("[BT_Service] 더 가까운 적 발견! 새 대상: %s (거리: %.2f)"), *OtherChar->GetName(), SurfaceDistance);
				MinDistance = SurfaceDistance;
				ClosestEnemy = OtherChar;
			}
		}
	}

	// [3단계] 최종 결과 반영 및 사거리 체크 로그
	if (ClosestEnemy)
	{
		BB->SetValueAsObject(TargetActorKey.SelectedKeyName, ClosestEnemy);
		BB->SetValueAsFloat(FName("DistanceToTarget"), MinDistance);

		float MyAttackRange = SelfUnit->GetAttackRange();

		if (MinDistance <= MyAttackRange)
		{
			// 🔍 로그: 공격 사거리 진입 성공 (초록색 느낌)
			UE_LOG(LogTemp, Error, TEXT("[BT_Service] %s ➔ %s : 사거리 진입! (거리: %.1f <= 사거리: %.1f)"),
				*SelfUnit->GetName(), *ClosestEnemy->GetName(), MinDistance, MyAttackRange);

			BB->SetValueAsBool(FName("bIsInRange"), true);
			BB->SetValueAsVector(FName("TargetLocation"), SelfUnit->GetActorLocation());
			AIC->SetFocus(ClosestEnemy);
		}
		else
		{
			// 🔍 로그: 아직 사거리 밖 (노란색 느낌)
			UE_LOG(LogTemp, Warning, TEXT("[BT_Service] %s ➔ %s : 접근 중... (거리: %.1f / 사거리: %.1f)"),
				*SelfUnit->GetName(), *ClosestEnemy->GetName(), MinDistance, MyAttackRange);

			BB->SetValueAsBool(FName("bIsInRange"), false);
			AIC->SetFocus(ClosestEnemy);

			FVector RandomOffset = FMath::VRand();
			RandomOffset.Z = 0.0f;
			RandomOffset.Normalize();

			FVector SpreadLocation = ClosestEnemy->GetActorLocation() + (RandomOffset * FMath::RandRange(50.0f, 100.0f));
			BB->SetValueAsVector(FName("TargetLocation"), SpreadLocation);
		}
	}
	else
	{
		// [4단계] 주변에 적 유닛이 없는 경우 (기지 이동 로직)

		// 🚨 [수정 핵심 2] 기지로 갈 때도 기존 시선(타겟)을 푸는 ClearFocus를 완전히 삭제합니다.
		// (코드 지움)

		BB->SetValueAsObject(TargetActorKey.SelectedKeyName, nullptr);
		BB->ClearValue(FName("DistanceToTarget"));
		BB->SetValueAsBool(FName("bIsInRange"), false);

		FVector CurrentTargetLoc = BB->GetValueAsVector(FName("TargetLocation"));
		AActor* DestBase = Cast<AActor>(BB->GetValueAsObject(FName("EnemyBaseActor")));

		// 🚨 [수정 핵심 3] 적이 없어서 기지로 향할 때는 시선을 기지(DestBase)로 고정합니다.
		if (DestBase)
		{
			AIC->SetFocus(DestBase);
		}

		if (DestBase && CurrentTargetLoc != FVector::ZeroVector)
		{
			float DistToBase = FVector::Dist(CurrentTargetLoc, DestBase->GetActorLocation());
			if (DistToBase < 1000.0f)
			{
				return;
			}
		}

		if (DestBase)
		{
			FVector BaseLocation = DestBase->GetActorLocation();
			FVector RandomOffset = FMath::VRand();
			RandomOffset.Z = 0.0f;
			RandomOffset.Normalize();

			float RandomRadius = FMath::RandRange(300.0f, 700.0f);
			FVector FinalTargetLocation = BaseLocation + (RandomOffset * RandomRadius);

			BB->SetValueAsVector(FName("TargetLocation"), FinalTargetLocation);
		}
	}
}
