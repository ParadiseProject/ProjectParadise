// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CostManageComponent.h"

UCostManageComponent::UCostManageComponent()
{
	// 매 프레임 Tick을 돌려야 함으로 true로 설정
	PrimaryComponentTick.bCanEverTick = true;

	//기본값 초기화
	MaxCost = 100.0f;
	CostRegenRate = 10.0f;
	CurrentCost = 100.0f;
	bIsRegenActive = false;	//게임 시작 전에는 멈춰둠
}


// Called when the game starts
void UCostManageComponent::BeginPlay()
{
	Super::BeginPlay();


	// 시작 시 현재 상태를 UI에 전파 (초기화)
	OnCostChanged.Broadcast(CurrentCost, MaxCost);
}

// Called every frame
void UCostManageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

		//1. 게임 시작 후에만 회복 시작
		if (!bIsRegenActive) return;

		//2. 최대치 도달 시 회복 중지
		if (CurrentCost >= MaxCost) return;

		//3. 코스트 회복 (초당 회복량 * 흐른 시작) 더하기
		CurrentCost += (CostRegenRate * DeltaTime);

		//4. 최대치 넘지 않도록 클램프
		CurrentCost = FMath::Clamp(CurrentCost, 0.0f, MaxCost);

		//디버그용 로그
		//UE_LOG(LogTemp, Warning, TEXT("현재 코스트: %f"), CurrentCost);

		//5. 값이 변할 떄마다 UI 갱신을 위해 델리게이트 호출
		OnCostChanged.Broadcast(CurrentCost, MaxCost);
}

//스위치 켜기
void UCostManageComponent::StartCostRegen()
{
	bIsRegenActive = true;
	//UE_LOG(LogTemp, Log, TEXT("💰 [CostManager] 코스트 회복 시작!"));

}

//스위치 끄기
void UCostManageComponent::StopCostRegen()
{
	bIsRegenActive = false;
	//UE_LOG(LogTemp, Log, TEXT("💰 [CostManager] 코스트 회복 중지!"));
}

// 코스트 소비 //유닛 소환 시 호출 (나중에 연결)
bool UCostManageComponent::TrySpendCost(float Amount)
{
	// 잔액 확인
	if (CurrentCost >= Amount)
	{
			CurrentCost -= Amount;

			// 깎인 후 즉시 UI 갱신 방송
			OnCostChanged.Broadcast(CurrentCost, MaxCost);

			// 성공 반환
			//UE_LOG(LogTemp, Log, TEXT("✅ [CostManager] 소비 성공: -%.1f (남은 코스트: %.1f)"), Amount, CurrentCost);
			return true;
	}

	//UE_LOG(LogTemp, Warning, TEXT("❌ [CostManager] 잔액 부족! (필요: %.1f, 보유: %.1f)"), Amount, CurrentCost);
	return false;
}

void UCostManageComponent::SetCurrentCost(float NewCost)
{
	CurrentCost = FMath::Clamp(NewCost, 0.0f, MaxCost);

	//변경된 값 즉시 방송
	OnCostChanged.Broadcast(CurrentCost, MaxCost);
}