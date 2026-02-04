// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Ingame/SkillSlotWidget.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "Engine/Texture2D.h"

void USkillSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

#pragma region 초기 UI 상태 설정
	// 최적화: 초기화 시점에 UI 요소를 비활성 상태로 세팅
	if (PB_Cooldown)
	{
		PB_Cooldown->SetPercent(0.f);
	}

	if (Text_CooldownTime)
	{
		Text_CooldownTime->SetVisibility(ESlateVisibility::Collapsed);
	}
#pragma endregion 초기 UI 상태 설정
}

void USkillSlotWidget::NativeDestruct()
{
#pragma region 리소스 정리
	StopCooldownTimer();
	Super::NativeDestruct();
#pragma endregion 리소스 정리
}

void USkillSlotWidget::UpdateSkillSlotData(UTexture2D* NewIcon, float InMaxCooldown)
{
#pragma region 캐릭터 태그 및 데이터 초기화
	/** * @brief 캐릭터 교체(Tag) 시 호출되어 슬롯의 데이터를 동적으로 주입받습니다.
	 * @details 기존 타이머를 즉시 정지시켜 이전 캐릭터의 데이터가 현재 캐릭터 UI에 영향을 주지 않도록 합니다.
	 */
	StopCooldownTimer();

	// 데이터 드라이븐: 데이터 테이블이나 외부에서 전달받은 스탯 주입
	MaxCooldownTime = InMaxCooldown;
	CurrentCooldownTime = 0.f;

	// 캡슐화된 이미지 컴포넌트에 텍스처 설정
	if (Image_Icon && NewIcon)
	{
		Image_Icon->SetBrushFromTexture(NewIcon);
	}

	// UI 초기 상태 리셋
	if (PB_Cooldown) PB_Cooldown->SetPercent(0.f);
	if (Text_CooldownTime) Text_CooldownTime->SetVisibility(ESlateVisibility::Collapsed);
#pragma endregion 캐릭터 태그 및 데이터 초기화
}

void USkillSlotWidget::SetCooldownStatus(float CooldownTime, float MaxTime)
{
#pragma region 타이머 로직 트리거
	CurrentCooldownTime = CooldownTime;
	MaxCooldownTime = MaxTime;

	if (CurrentCooldownTime > 0.f)
	{
		// 최적화: 타이머가 이미 돌고 있다면 중복 생성하지 않음
		if (!GetWorld()->GetTimerManager().IsTimerActive(CooldownTimerHandle))
		{
			// 데이터 드라이븐: 기획자가 설정한 UpdateInterval 주기를 참조하여 타이머 시작
			GetWorld()->GetTimerManager().SetTimer(
				CooldownTimerHandle,
				this,
				&USkillSlotWidget::UpdateCooldownVisual,
				UpdateInterval,
				true
			);
		}

		if (Text_CooldownTime)
		{
			Text_CooldownTime->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
	else
	{
		StopCooldownTimer();
	}
#pragma endregion 타이머 로직 트리거
}

void USkillSlotWidget::UpdateCooldownVisual()
{
#pragma region 실시간 시각화 갱신
	/**
	 * @brief 타이머 주기에 맞춰 호출되어 ProgressBar와 Text를 갱신합니다.
	 * @details 매 프레임 연산하는 Tick보다 훨씬 가벼우며, 정해진 주기(UpdateInterval)만큼만 계산합니다.
	 */
	CurrentCooldownTime -= UpdateInterval;

	// 쿨타임 종료 판정
	if (CurrentCooldownTime <= 0.f)
	{
		StopCooldownTimer();
		return;
	}

	// 캡슐화된 위젯 컴포넌트 안전성 체크 후 갱신
	if (PB_Cooldown && MaxCooldownTime > 0.f)
	{
		PB_Cooldown->SetPercent(CurrentCooldownTime / MaxCooldownTime);
	}

	if (Text_CooldownTime)
	{
		// 올림(Ceil) 처리를 통해 유저에게 남은 초를 직관적으로 표시
		Text_CooldownTime->SetText(FText::AsNumber(FMath::CeilToInt(CurrentCooldownTime)));
	}
#pragma endregion 실시간 시각화 갱신
}

void USkillSlotWidget::StopCooldownTimer()
{
#pragma region 자원 해제 및 정리
	/**
	 * @brief 활성화된 타이머를 중지하고 쿨타임 UI를 초기화합니다.
	 */
	if (GetWorld() && CooldownTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
	}

	if (PB_Cooldown)
	{
		PB_Cooldown->SetPercent(0.f);
	}

	if (Text_CooldownTime)
	{
		Text_CooldownTime->SetVisibility(ESlateVisibility::Collapsed);
	}
#pragma endregion 자원 해제 및 정리
}
