// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Panel/Ingame/SummonControlPanel.h"

#include "UI/Widgets/Ingame/SummonSlotWidget.h"
#include "UI/Widgets/Ingame/SummonCostWidget.h"
#include "Components/CostManageComponent.h"
#include "GameFramework/PlayerController.h"
#include "Framework/InGame/InGamePlayerState.h"

#pragma region 생명주기
void USummonControlPanel::NativeConstruct()
{
	Super::NativeConstruct();

	// 배열에 캐싱하여 인덱스로 접근 가능하게 최적화
	SummonSlots.Empty();
	if (SummonSlot_0) SummonSlots.Add(SummonSlot_0);
	if (SummonSlot_1) SummonSlots.Add(SummonSlot_1);
	if (SummonSlot_2) SummonSlots.Add(SummonSlot_2);
	if (SummonSlot_3) SummonSlots.Add(SummonSlot_3);
	if (SummonSlot_4) SummonSlots.Add(SummonSlot_4); 

	InitCostSystem();
}
void USummonControlPanel::NativeDestruct()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_InitCost);
	}
	if (CachedCostComponent.IsValid())
	{
		if (CachedCostComponent->OnCostChanged.IsAlreadyBound(this, &USummonControlPanel::HandleCostUpdate))
		{
			CachedCostComponent->OnCostChanged.RemoveDynamic(this, &USummonControlPanel::HandleCostUpdate);
		}
	}
	Super::NativeDestruct();
}
void USummonControlPanel::InitCostSystem()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		// PC가 아직 없으면 다음 프레임에 재시도
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_InitCost, this, &USummonControlPanel::InitCostSystem, 0.1f, false);
		return;
	}

	AInGamePlayerState* PS = PC->GetPlayerState<AInGamePlayerState>();
	if (!PS)
	{
		// PlayerState가 아직 생성/복제되지 않았으면 재시도
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_InitCost, this, &USummonControlPanel::InitCostSystem, 0.1f, false);
		return;
	}

	// PlayerState를 찾았으니 컴포넌트 가져오기
	UCostManageComponent* CostComp = PS->GetCostManageComponent();
	if (CostComp)
	{
		CachedCostComponent = CostComp;

		// 1. 델리게이트 바인딩
		if (!CostComp->OnCostChanged.IsAlreadyBound(this, &USummonControlPanel::HandleCostUpdate))
		{
			CostComp->OnCostChanged.AddDynamic(this, &USummonControlPanel::HandleCostUpdate);
		}

		// ★ 성공했으므로 타이머 종료 (더 이상 재시도 안 함)
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_InitCost);
		UE_LOG(LogTemp, Log, TEXT("[Panel] 코스트 시스템 정상 연결됨"));
	}
	else
	{
		// PS는 있는데 컴포넌트가 null인 경우 (드물지만 재시도)
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_InitCost, this, &USummonControlPanel::InitCostSystem, 0.1f, false);
	}
}
void USummonControlPanel::HandleCostUpdate(float CurrentCost, float MaxCost)
{
	if (CostWidget)
	{
		CostWidget->UpdateCost(CurrentCost, MaxCost);
	}
}
#pragma endregion 생명주기

#pragma region 외부 인터페이스 구현
void USummonControlPanel::SetSummonSlotData(int32 SlotIndex, UTexture2D* Icon, float MaxCooldown)
{
	if (SummonSlots.IsValidIndex(SlotIndex) && SummonSlots[SlotIndex])
	{
		SummonSlots[SlotIndex]->UpdateSummonData(Icon, MaxCooldown);
	}
}

void USummonControlPanel::UpdateSummonCooldown(int32 SlotIndex, float CurrentTime, float MaxTime)
{
	if (SummonSlots.IsValidIndex(SlotIndex) && SummonSlots[SlotIndex])
	{
		SummonSlots[SlotIndex]->RefreshCooldown(CurrentTime, MaxTime);
	}
}
void USummonControlPanel::UpdateCostDisplay(float CurrentCost, float MaxCost)
{
	if (CostWidget)
	{
		CostWidget->UpdateCost(CurrentCost, MaxCost);
	}
}
#pragma endregion 외부 인터페이스 구현
