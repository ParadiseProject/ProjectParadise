// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Panel/Ingame/SummonControlPanel.h"

#include "UI/Widgets/Ingame/SummonSlotWidget.h"
#include "UI/Widgets/Ingame/SummonCostWidget.h"

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
