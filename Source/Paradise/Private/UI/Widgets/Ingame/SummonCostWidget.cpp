// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Ingame/SummonCostWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

USummonCostWidget::USummonCostWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

#pragma region 생명주기
void USummonCostWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 초기화
	UpdateCost(0.0f, 10.0f);
}
#pragma endregion 생명주기

#pragma region 외부 인터페이스 구현
void USummonCostWidget::UpdateCost(float CurrentCost, float MaxCost)
{
	if (MaxCost <= 0.0f) return;

	const float Percent = FMath::Clamp(CurrentCost / MaxCost, 0.0f, 1.0f);

	// 1. 프로그레스 바 갱신
	if (PB_CostGauge)
	{
		PB_CostGauge->SetPercent(Percent);
	}

	// 2. 텍스트 갱신 (소수점 1자리까지 표시 or 정수 표시)
	if (Text_CostValue)
	{
		// 예: "4 / 10" 형태로 표시 (4.9여도 4로 표시됨 -> 유저 혼동 방지)
		FString CostStr = FString::Printf(TEXT("%d / %d"), FMath::FloorToInt(CurrentCost), FMath::FloorToInt(MaxCost));
		Text_CostValue->SetText(FText::FromString(CostStr));
	}
}
#pragma endregion 외부 인터페이스 구현
