// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Squad/Inventory/ParadiseSquadInventoryWidget.h"
#include "UI/Widgets/Squad/Inventory/ParadiseItemSlot.h"
#include "Components/WidgetSwitcher.h"
#include "Components/WrapBox.h"

#pragma region 공개 함수
void UParadiseSquadInventoryWidget::UpdateList(int32 TabIndex, const TArray<FSquadItemUIData>& ListData)
{
	if (!Switcher_List) return;

	Switcher_List->SetActiveWidgetIndex(TabIndex);

	UWrapBox* TargetWrap = nullptr;
	switch (TabIndex)
	{
	case SquadTabs::Character: TargetWrap = Wrap_Character; break;
	case SquadTabs::Weapon:    TargetWrap = Wrap_Weapon; break;
	case SquadTabs::Armor:     TargetWrap = Wrap_Armor; break;
	case SquadTabs::Unit:      TargetWrap = Wrap_Unit; break;
	}

	if (!TargetWrap || !ItemSlotClass) return;

	TargetWrap->ClearChildren();

	for (const auto& Data : ListData)
	{
		// 슬롯 생성 (UUserWidget을 상속받은 커스텀 슬롯으로 캐스팅하여 사용)
		UUserWidget* NewSlot = CreateWidget<UUserWidget>(this, ItemSlotClass);

		// TODO: 실제 구현 시 IParadiseSlotInterface 등을 통해 SetData 호출
		// if (auto* Slot = Cast<UParadiseItemSlot>(NewSlot)) { Slot->SetData(Data); }

		TargetWrap->AddChildToWrapBox(NewSlot);
	}
}
#pragma endregion 공개 함수

#pragma region 내부 로직
void UParadiseSquadInventoryWidget::HandleSlotClick(FSquadItemUIData Data)
{
	// 클릭된 데이터를 상위로 전파
	OnItemClicked.Broadcast(Data);
}
#pragma endregion 내부 로직