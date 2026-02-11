// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Squad/Inventory/ParadiseItemSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UParadiseItemSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Select)
	{
		Btn_Select->OnClicked.AddDynamic(this, &UParadiseItemSlot::OnButtonClicked);
	}
}

void UParadiseItemSlot::NativeDestruct()
{
	if (Btn_Select)
	{
		Btn_Select->OnClicked.RemoveAll(this);
	}
	Super::NativeDestruct();
}

void UParadiseItemSlot::UpdateSlot(const FSquadItemUIData& InData)
{
	CachedData = InData;
	CachedID = InData.ID;

	// 1. 아이콘 설정
	if (Img_Icon)
	{
		if (InData.Icon)
		{
			Img_Icon->SetBrushFromTexture(InData.Icon);
			Img_Icon->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			// 아이콘 없으면 투명 처리 혹은 기본 이미지
			Img_Icon->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	// 2. 레벨 텍스트
	if (Text_Level)
	{
		if (InData.LevelOrCount > 0)
		{
			Text_Level->SetText(FText::AsNumber(InData.LevelOrCount));
			Text_Level->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			Text_Level->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// 3. 등급 테두리 색상
	UpdateRankColor(InData.RankTag);

	// 4. 장착 표시
	if (Img_EquippedMark)
	{
		Img_EquippedMark->SetVisibility(InData.bIsEquipped ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UParadiseItemSlot::OnButtonClicked()
{
	// 상위 위젯(InventoryPanel)에게 클릭 사실 전파
	OnSlotClicked.Broadcast(CachedData);
}

void UParadiseItemSlot::UpdateRankColor(FGameplayTag RankTag)
{
	if (!Img_RankBorder) return;

	FLinearColor BorderColor = FLinearColor::White; // 기본값

	// 태그 매칭 로직 (프로젝트 규칙에 맞게 수정)
	if (RankTag.MatchesTag(FGameplayTag::RequestGameplayTag("Unit.Rank.S")))
	{
		BorderColor = FLinearColor(1.0f, 0.8f, 0.0f); // Gold
	}
	else if (RankTag.MatchesTag(FGameplayTag::RequestGameplayTag("Unit.Rank.A")))
	{
		BorderColor = FLinearColor(0.8f, 0.0f, 1.0f); // Purple
	}
	// ... 기타 등급 처리

	Img_RankBorder->SetColorAndOpacity(BorderColor);
}
