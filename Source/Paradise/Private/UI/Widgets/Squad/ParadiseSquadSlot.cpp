// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Squad/ParadiseSquadSlot.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

#pragma region 생명주기
void UParadiseSquadSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Select)
	{
		Btn_Select->OnClicked.AddDynamic(this, &UParadiseSquadSlot::OnButtonClicked);
	}

	// 초기엔 선택 안 된 상태
	if (Img_SelectionBorder)
	{
		Img_SelectionBorder->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UParadiseSquadSlot::NativeDestruct()
{
	if (Btn_Select)
	{
		Btn_Select->OnClicked.RemoveAll(this);
	}
	Super::NativeDestruct();
}
#pragma endregion 생명주기

#pragma region 로직 구현
void UParadiseSquadSlot::InitSlot(int32 InSlotIndex)
{
	SlotIndex = InSlotIndex;
}

void UParadiseSquadSlot::UpdateSlot(const FSquadItemUIData& InData)
{
	// ID가 없으면 비어있는 슬롯으로 간주
	bIsEmpty = InData.ID.IsNone();

	if (bIsEmpty)
	{
		// 1. 비어있음 (Empty)
		if (Img_Icon) Img_Icon->SetVisibility(ESlateVisibility::Collapsed);
		if (Img_EmptyPlaceholder) Img_EmptyPlaceholder->SetVisibility(ESlateVisibility::Visible);
		if (Text_Level) Text_Level->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		// 2. 채워짐 (Filled)
		if (Img_Icon)
		{
			Img_Icon->SetBrushFromTexture(InData.Icon);
			Img_Icon->SetVisibility(ESlateVisibility::Visible);
		}

		if (Img_EmptyPlaceholder) Img_EmptyPlaceholder->SetVisibility(ESlateVisibility::Collapsed);

		if (Text_Level)
		{
			Text_Level->SetText(FText::AsNumber(InData.LevelOrCount));
			Text_Level->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UParadiseSquadSlot::SetSelected(bool bIsSelected)
{
	if (Img_SelectionBorder)
	{
		Img_SelectionBorder->SetVisibility(bIsSelected ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UParadiseSquadSlot::OnButtonClicked()
{
	if (SlotIndex != -1)
	{
		OnSlotClicked.Broadcast(SlotIndex);
	}
}
#pragma endregion 로직 구현