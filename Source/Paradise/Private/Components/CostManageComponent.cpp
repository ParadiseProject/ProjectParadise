// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CostManageComponent.h"
#include "Net/UnrealNetwork.h"

UCostManageComponent::UCostManageComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	MaxCost = 100.0f;
	CostRegenRate = 10.0f;
	CurrentCost = 0.0f;
	bIsRegenActive = false;	//게임 시작 전에는 멈춰둠
}


// Called when the game starts
void UCostManageComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	OnCostChanged.Broadcast(CurrentCost, MaxCost);
}

void UCostManageComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

// Called every frame
void UCostManageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		//1. 게임 시작 후에만 회복 시작
		if (!bIsRegenActive) return;

		//2. 최대치 도달 시 회복 중지
		if (CurrentCost >= MaxCost) return;

		//3. 코스트 회복 (초당 회복량 * 흐른 시작) 더하기
		CurrentCost += (CostRegenRate * DeltaTime);

		//4. 최대치 넘지 않도록 클램프
		CurrentCost = FMath::Clamp(CurrentCost, 0.0f, MaxCost);

		//5. 값이 변할 떄마다 UI 갱신을 위해 델리게이트 호출
		OnCostChanged.Broadcast(CurrentCost, MaxCost);
	}
}

//스위치 켜기
void UCostManageComponent::StartCostRegen()
{
	bIsRegenActive = true;
	UE_LOG(LogTemp, Log, TEXT("💰 [CostManager] 코스트 회복 시작!"));

}

//스위치 끄기
void UCostManageComponent::StopCostRegen()
{
	bIsRegenActive = false;
}

// 코스트 소비 //유닛 소환 시 호출
bool UCostManageComponent::TrySpendCost(float Amount)
{
	// 서버만 돈을 깎을 수 있음
	if (GetOwner() && !GetOwner()->HasAuthority()) return false;

	// 잔액 확인
	if (CurrentCost >= Amount)
	{
		CurrentCost -= Amount;

		// 깎인 후 즉시 UI 갱신 방송
		OnCostChanged.Broadcast(CurrentCost, MaxCost);

		// 성공 반환
		return true;
	}

	// 잔액 부족
	return false;
}

void UCostManageComponent::OnRep_CurrentCost()
{

}
