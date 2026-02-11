// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Squad/Inventory/ParadiseSquadInventoryWidget.h"
#include "UI/Widgets/Squad/Inventory/ParadiseItemSlot.h"
#include "Components/WidgetSwitcher.h"
#include "Components/WrapBox.h"

#pragma region 공개 함수
void UParadiseSquadInventoryWidget::UpdateList(int32 TabIndex, const TArray<FSquadItemUIData>& ListData)
{
	if (!Switcher_List) return;

	// 1. 탭 전환
	Switcher_List->SetActiveWidgetIndex(TabIndex);

	// 2. 현재 탭에 맞는 WrapBox 찾기
	UWrapBox* TargetWrap = nullptr;
	switch (TabIndex)
	{
	case SquadTabs::Character: TargetWrap = Wrap_Character; break;
	case SquadTabs::Weapon:    TargetWrap = Wrap_Weapon; break;
	case SquadTabs::Armor:     TargetWrap = Wrap_Armor; break;
	case SquadTabs::Unit:      TargetWrap = Wrap_Unit; break;
	}

	// 방어 코드: WrapBox나 슬롯 클래스가 없으면 중단
	if (!TargetWrap || !ItemSlotClass) return;

	// 3. 기존 슬롯 제거 (초기화)
	TargetWrap->ClearChildren();

	// 4. 데이터 기반 슬롯 생성 (Loop)
	for (const auto& Data : ListData)
	{
		// 슬롯 생성 (UUserWidget을 상속받은 커스텀 슬롯으로 캐스팅하여 사용)
		if (UParadiseItemSlot* NewSlot = CreateWidget<UParadiseItemSlot>(this, ItemSlotClass))
		{
			// (1) 데이터 주입
			NewSlot->UpdateSlot(Data);

			// (2) 클릭 이벤트 바인딩 (내부 핸들러로 연결)
			NewSlot->OnSlotClicked.AddDynamic(this, &UParadiseSquadInventoryWidget::HandleSlotClick);

			// (3) 화면(WrapBox)에 추가
			TargetWrap->AddChildToWrapBox(NewSlot);
		}
	}
}
#pragma endregion 공개 함수

#pragma region 내부 로직
void UParadiseSquadInventoryWidget::HandleSlotClick(FSquadItemUIData ItemData)
{
	// 클릭된 데이터를 상위로 전파
	if (OnItemClicked.IsBound())
	{
		OnItemClicked.Broadcast(ItemData);
	}
}
#pragma endregion 내부 로직