// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Unit/BTService_LaneTargeting.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Objects/HomeBase.h" 
#include "Characters/AIUnit/UnitBase.h" // AUnitBase 및 IsEnemy 함수 사용을 위해 포함
#include "Characters/Base/CharacterBase.h"
#include "Engine/Engine.h" // 🚨 로그 출력을 위해 추가

UBTService_LaneTargeting::UBTService_LaneTargeting()
{
	NodeName = TEXT("Lane Targeting");
	bNotifyTick = true;
	Interval = 0.5f;
	RandomDeviation = 0.1f;

	TargetObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
}

void UBTService_LaneTargeting::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!BBComp || !AICon || !AICon->GetPawn()) return;

	APawn* ControlledPawn = AICon->GetPawn();
	AUnitBase* MyUnit = Cast<AUnitBase>(ControlledPawn);
	if (!MyUnit) return;

	FVector MyLocation = ControlledPawn->GetActorLocation();

	AActor* ExistingTarget = Cast<AActor>(BBComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
	AUnitBase* ExistingEnemy = Cast<AUnitBase>(ExistingTarget);

	bool bNeedSearch = true; // 새로 탐색을 해야 하는가?

	// 1. 기존 타겟 유지 검사 (록온)
	if (ExistingEnemy && !ExistingEnemy->IsDead())
	{
		if (MyUnit->GetDistanceTo(ExistingEnemy) <= SearchRadius)
		{
			// 내 반경 안에 이미 물고 있는 적이 있다면, 색적을 건너뜀 (return 하지 않고 플래그만 변경!)
			bNeedSearch = false;
		}
	}

	// 2. 주변 탐색 (새로운 적을 찾아야 할 때만 실행)
	if (bNeedSearch)
	{
		TArray<AActor*> OutActors;
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(ControlledPawn);

		UKismetSystemLibrary::SphereOverlapActors(GetWorld(), MyLocation, SearchRadius, TargetObjectTypes, nullptr, ActorsToIgnore, OutActors);

		AActor* ClosestEnemy = nullptr;
		float MinDistanceSq = MAX_flt;

		for (AActor* OverlappedActor : OutActors)
		{
			AUnitBase* TargetUnit = Cast<AUnitBase>(OverlappedActor);
			if (!TargetUnit || TargetUnit->IsDead() || !MyUnit->IsEnemy(TargetUnit)) continue;

			float DistanceSq = FVector::DistSquared(MyLocation, OverlappedActor->GetActorLocation());
			if (DistanceSq < MinDistanceSq)
			{
				MinDistanceSq = DistanceSq;
				ClosestEnemy = OverlappedActor;
			}
		}

		if (ClosestEnemy)
		{
			BBComp->SetValueAsObject(TargetActorKey.SelectedKeyName, ClosestEnemy);
		}
		else
		{
			AActor* EnemyBase = Cast<AActor>(BBComp->GetValueAsObject(EnemyBaseKey.SelectedKeyName));
			if (!EnemyBase)
			{
				TArray<AActor*> FoundBases;
				UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHomeBase::StaticClass(), FoundBases);
				for (AActor* BaseActor : FoundBases)
				{
					AHomeBase* HomeBase = Cast<AHomeBase>(BaseActor);
					if (HomeBase && MyUnit->IsEnemy(HomeBase))
					{
						EnemyBase = HomeBase;
						BBComp->SetValueAsObject(EnemyBaseKey.SelectedKeyName, EnemyBase);
						break;
					}
				}
			}
			BBComp->SetValueAsObject(TargetActorKey.SelectedKeyName, EnemyBase);
		}
	}

	// =========================================================
	// 🚨 3. [핵심] 모든 상황에서 무조건 마지막에 실행되는 사거리 체크
	// =========================================================
	AActor* FinalTarget = Cast<AActor>(BBComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (FinalTarget)
	{
		// 유닛(MyUnit)에서 직접 사거리를 가져옵니다!
		float AttackRange = MyUnit->GetAttackRange();

		// 혹시라도 사거리 데이터가 안 들어왔을 때를 대비한 방어 코드
		if (AttackRange <= 0.0f) AttackRange = 150.0f;

		// =========================================================
			// 🚨 [수정] Z축 높이 오차를 없앤 완벽한 2D 수평 사거리 계산
			// =========================================================

			// 기존 코드: float CenterDist = MyUnit->GetDistanceTo(FinalTarget); (3D 거리라 오차 발생)

			// 수정 코드: Z축을 무시하고 바닥(XY 평면)을 기준으로 한 수평 거리만 잽니다.
		float CenterDist = MyUnit->GetHorizontalDistanceTo(FinalTarget);

		float MyRadius = MyUnit->GetSimpleCollisionRadius();
		float TargetRadius = FinalTarget->GetSimpleCollisionRadius();

		// 이제 맞닿으면 정확하게 0(또는 거의 0에 가까운 소수점)이 나옵니다!
		float EdgeDist = FMath::Max(0.0f, CenterDist - MyRadius - TargetRadius);

		// 사거리 진입 여부 판단 (오차 허용치 30.0f)
		float AllowedRange = AttackRange + 30.0f;
		bool bInRange = EdgeDist <= AllowedRange;

		// 블랙보드 업데이트
		BBComp->SetValueAsBool(FName("bIsTargetInRange"), bInRange);

		// =========================================================
		// 🚨 [디버그 로그] 사거리 계산 결과 실시간 확인
		// =========================================================
		//if (GEngine)
		//{
		//	FString LogMsg = FString::Printf(TEXT("[%s] 🎯 대상: %s | 내사거리: %.1f | 실제거리(Edge): %.1f | 허용거리: %.1f | 결과: %s"),
		//		*MyUnit->GetName(),
		//		*FinalTarget->GetName(),
		//		AttackRange,
		//		EdgeDist,
		//		AllowedRange,
		//		bInRange ? TEXT("TRUE (사거리 안)") : TEXT("FALSE (사거리 밖)"));

		//	// 범위 안이면 초록색, 밖이면 주황색으로 0.5초간 출력
		//	GEngine->AddOnScreenDebugMessage(-1, 0.5f, bInRange ? FColor::Green : FColor::Orange, LogMsg);
		//}
	}

}