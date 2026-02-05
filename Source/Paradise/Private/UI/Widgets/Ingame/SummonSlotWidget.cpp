// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Ingame/SummonSlotWidget.h"

#include "UI/Widgets/Ingame/ParadiseCommonButton.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"

USummonSlotWidget::USummonSlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

#pragma region 생명주기
void USummonSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 입력은 버튼에게 위임하고, 위젯은 로직만 처리합니다.
	if (Btn_SummonAction)
	{
		Btn_SummonAction->OnClicked().AddUObject(this, &USummonSlotWidget::OnSummonButtonClicked);
	}

	StopCooldownTimer();
}

void USummonSlotWidget::NativeDestruct()
{
	StopCooldownTimer();
	Super::NativeDestruct();
}
#pragma endregion 생명주기

#pragma region 외부 인터페이스 구현
void USummonSlotWidget::UpdateSummonData(UTexture2D* IconTexture, float InMaxCooldown)
{
	StopCooldownTimer();
	MaxCooldownTime = InMaxCooldown;

	if (Img_SummonIcon)
	{
		if (IconTexture)
		{
			Img_SummonIcon->SetBrushFromTexture(IconTexture);
			Img_SummonIcon->SetVisibility(ESlateVisibility::HitTestInvisible); // 클릭 통과
			
			if (Btn_SummonAction) Btn_SummonAction->SetIsEnabled(true);
		}
		else
		{
			Img_SummonIcon->SetVisibility(ESlateVisibility::Hidden);
			// 데이터가 없으면 비활성화 (빈 슬롯)
			if(Btn_SummonAction) Btn_SummonAction->SetIsEnabled(false);
		}
	}
}

void USummonSlotWidget::RefreshCooldown(float CurrentTime, float MaxTime)
{
	CurrentCooldownTime = CurrentTime;
	MaxCooldownTime = MaxTime;

	if (CurrentCooldownTime > 0.0f)
	{
		// 쿨타임 시작: 버튼 비활성화 및 타이머 가동
		if (Btn_SummonAction) Btn_SummonAction->SetIsEnabled(false);

		if (PB_Cooldown) PB_Cooldown->SetVisibility(ESlateVisibility::HitTestInvisible);
		if (Text_CooldownTime) Text_CooldownTime->SetVisibility(ESlateVisibility::HitTestInvisible);

		if (GetWorld() && !GetWorld()->GetTimerManager().IsTimerActive(CooldownTimerHandle))
		{
			GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, this, &USummonSlotWidget::UpdateCooldownVisual, UpdateInterval, true);
		}
	}
	else
	{
		StopCooldownTimer();
	}
}
#pragma endregion 외부 인터페이스 구현

#pragma region 내부 로직
void USummonSlotWidget::OnSummonButtonClicked()
{
	// 실제 소환 로직 요청 (델리게이트 브로드캐스트 등)
	// 예: OnSummonRequested.Broadcast();
}

void USummonSlotWidget::UpdateCooldownVisual()
{
	CurrentCooldownTime -= UpdateInterval;

	if (CurrentCooldownTime <= 0.0f)
	{
		StopCooldownTimer();
		return;
	}

	if (PB_Cooldown && MaxCooldownTime > 0.0f)
	{
		PB_Cooldown->SetPercent(CurrentCooldownTime / MaxCooldownTime);
	}

	if (Text_CooldownTime)
	{
		Text_CooldownTime->SetText(FText::AsNumber(FMath::CeilToInt(CurrentCooldownTime)));
	}
}

void USummonSlotWidget::StopCooldownTimer()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
	}

	if (PB_Cooldown)
	{
		PB_Cooldown->SetPercent(0.0f);
		PB_Cooldown->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (Text_CooldownTime)
	{
		Text_CooldownTime->SetVisibility(ESlateVisibility::Collapsed);
	}

	// 쿨타임 종료 시 버튼 활성화 (단, 아이콘이 있어야 함)
	if (Btn_SummonAction && Img_SummonIcon && Img_SummonIcon->GetVisibility() != ESlateVisibility::Hidden)
	{
		Btn_SummonAction->SetIsEnabled(true);
	}
}
#pragma endregion 내부 로직
