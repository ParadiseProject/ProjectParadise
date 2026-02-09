// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Ingame/Result/ResultCharacterSlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UResultCharacterSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UResultCharacterSlotWidget::SetSlotData(const FResultCharacterData& InData)
{
	// 1. 초상화 설정
	if (Img_Portrait && InData.PortraitImage)
	{
		Img_Portrait->SetBrushFromTexture(InData.PortraitImage);
	}

	// 2. 이름 설정
	if (Text_Name)
	{
		Text_Name->SetText(InData.CharacterName);
	}

	// 3. 경험치 텍스트 (+150 Exp)
	if (Text_GainedExp)
	{
		FString ExpString = FString::Printf(TEXT("+%d EXP"), InData.GainedExp);
		Text_GainedExp->SetText(FText::FromString(ExpString));
	}

	// 4. 경험치 바 (비율)
	if (ProgressBar_Exp)
	{
		ProgressBar_Exp->SetPercent(InData.ExpPercent);
	}
}
