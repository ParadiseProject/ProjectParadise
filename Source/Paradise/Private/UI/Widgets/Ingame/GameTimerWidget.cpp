// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Ingame/GameTimerWidget.h"

#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/InGame/InGameGameState.h"

void UGameTimerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CachedSeconds = -1; // 강제 갱신을 위해 초기화
	
	// GameState 가져오기
	CachedGameState = Cast<AInGameGameState>(UGameplayStatics::GetGameState(this));
	if (CachedGameState)
	{
		// 1. 델리게이트 바인딩 (int32끼리 연결)
		if (!CachedGameState->OnTimerChanged.IsAlreadyBound(this, &UGameTimerWidget::UpdateTime))
		{
			CachedGameState->OnTimerChanged.AddDynamic(this, &UGameTimerWidget::UpdateTime);
		}
		// 2. 초기값 설정 (UI가 뜨자마자 00:00이 아니라 현재 시간으로 보이게)
		UpdateTime((int32)CachedGameState->RemainingTime);
	}
}

void UGameTimerWidget::NativeDestruct()
{
	if (CachedGameState && CachedGameState->OnTimerChanged.IsBound())
	{
		CachedGameState->OnTimerChanged.RemoveDynamic(this, &UGameTimerWidget::UpdateTime);
	}

	Super::NativeDestruct();
}

void UGameTimerWidget::UpdateTime(int32 NewTime)
{
	// 최적화: 초 단위가 바뀌었을 때만 텍스트 갱신
	if (NewTime != CachedSeconds)
	{
		CachedSeconds = NewTime;

		if (Text_TimeValue)
		{
			Text_TimeValue->SetText(GetFormattedTime(NewTime));

			// 10초 이하일 때 빨간색 경고
			if (NewTime <= WarningThreshold && NewTime > 0)
			{
				Text_TimeValue->SetColorAndOpacity(FLinearColor::Red);
			}
		}
	}
}

FText UGameTimerWidget::GetFormattedTime(int32 TimeInSeconds) const
{
	// 음수 방지
	const int32 TotalSeconds = FMath::Max(0, TimeInSeconds);

	const int32 Minutes = TotalSeconds / 60;
	const int32 Seconds = TotalSeconds % 60;

	// "05:00" 형식으로 포맷팅
	return FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds));
}