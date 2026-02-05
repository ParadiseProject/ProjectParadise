// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Ingame/GameTimerWidget.h"

#include "Components/TextBlock.h"

void UGameTimerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CachedSeconds = -1; // 강제 갱신을 위해 초기화
}

void UGameTimerWidget::UpdateTime(float CurrentTime)
{
	// 올림 처리 (0.1초 남았을 때 0초로 보이면 안 되므로)
	const int32 NewSeconds = FMath::CeilToInt(CurrentTime);

	// 최적화: 초 단위가 바뀌었을 때만 텍스트 갱신
	if (NewSeconds != CachedSeconds)
	{
		CachedSeconds = NewSeconds;

		if (Text_TimeValue)
		{
			Text_TimeValue->SetText(GetFormattedTime(CurrentTime));

			// 10초 이하일 때 빨간색 경고
			if (CurrentTime <= WarningThreshold && CurrentTime > 0.0f)
			{
				Text_TimeValue->SetColorAndOpacity(FLinearColor::Red);
			}
			else
			{
				Text_TimeValue->SetColorAndOpacity(FLinearColor::White);
			}
		}
	}
}

FText UGameTimerWidget::GetFormattedTime(float TimeInSeconds) const
{
	const int32 TotalSeconds = FMath::Max(0, FMath::CeilToInt(TimeInSeconds));
	const int32 Minutes = TotalSeconds / 60;
	const int32 Seconds = TotalSeconds % 60;

	// "05:00" 형식으로 포맷팅
	return FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds));
}