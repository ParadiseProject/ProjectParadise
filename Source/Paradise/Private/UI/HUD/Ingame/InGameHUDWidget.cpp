// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/Ingame/InGameHUDWidget.h"

#include "UI/Widgets/Ingame/ParadiseCommonButton.h"
#include "UI/Widgets/Ingame/VirtualJoystickWidget.h"
#include "UI/Panel/Ingame/ActionControlPanel.h"
#include "UI/Panel/Ingame/PartyStatusPanel.h"
#include "UI/Panel/Ingame/SummonControlPanel.h"
#include "UI/Widgets/Ingame/CharacterStatusWidget.h"
#include "UI/Widgets/Ingame/GameTimerWidget.h"
#include "UI/Widgets/Ingame/Popup/ResultPopupWidget.h"
#include "Framework/InGame/InGameGameState.h"

#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void UInGameHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 설정 버튼 이벤트 바인딩
	if (Btn_Setting)
	{
		Btn_Setting->OnClicked().AddUObject(this, &UInGameHUDWidget::OnSettingButtonClicked);
	}

	// 자동 전투 버튼 이벤트 바인딩
	if (Btn_AutoMode)
	{
		Btn_AutoMode->OnClicked().AddUObject(this, &UInGameHUDWidget::OnAutoModeButtonClicked);

		// 초기 상태 설정 (예: OFF 텍스트 또는 회색 아이콘)
		// Btn_AutoMode->SetButtonText(FText::FromString(TEXT("MANUAL")));
	}

	// 가상 조이스틱 이벤트 바인딩
	if (VirtualJoystick)
	{
		// 조이스틱의 델리게이트와 HUD의 이동 처리 함수 연결
		VirtualJoystick->OnJoystickInput.AddDynamic(this, &UInGameHUDWidget::OnJoystickInput);
	}

	// 결과 팝업 초기화 (숨김)
	if (Widget_ResultPopup)
	{
		Widget_ResultPopup->SetVisibility(ESlateVisibility::Collapsed);
	}

	// GameState 연결 및 델리게이트 구독
	InitializeHUD();

	// Tick 대신 0.5초 주기로 UI 갱신 타이머 실행
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			HUDUpdateTimerHandle,
			this,
			&UInGameHUDWidget::OnUpdateHUD,
			0.5f, // 0.5초마다 갱신
			true
		);
	}

}

void UInGameHUDWidget::NativeDestruct()
{
	// 1. 타이머 정지
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(HUDUpdateTimerHandle);
	}

	// 2. 델리게이트 해제 (안전장치)
	AInGameGameState* GS = Cast<AInGameGameState>(UGameplayStatics::GetGameState(this));
	if (GS)
	{
		GS->OnGamePhaseChanged.RemoveAll(this);
	}

	Super::NativeDestruct();
}

void UInGameHUDWidget::InitializeHUD()
{
	UE_LOG(LogTemp, Log, TEXT("InGameHUD 초기화"));

	AInGameGameState* GS = Cast<AInGameGameState>(UGameplayStatics::GetGameState(this));
	if (GS)
	{
		// 페이즈 변경 이벤트 바인딩
		GS->OnGamePhaseChanged.AddUniqueDynamic(this, &UInGameHUDWidget::HandleGamePhaseChanged);

		// 현재 상태 즉시 반영 (이미 게임 중일 수 있음)
		HandleGamePhaseChanged(GS->CurrentPhase);
	}
}

void UInGameHUDWidget::HandleGamePhaseChanged(EGamePhase NewPhase)
{
	UE_LOG(LogTemp, Log, TEXT("[InGameHUD] 페이즈 변경 감지: %d"), (int32)NewPhase);

	switch (NewPhase)
	{
	case EGamePhase::Victory:
	case EGamePhase::Defeat:
		// 결과창 표시
		if (Widget_ResultPopup)
		{
			AInGameGameState* GS = Cast<AInGameGameState>(UGameplayStatics::GetGameState(this));
			if (GS)
			{
				bool bIsWin = (NewPhase == EGamePhase::Victory);
				Widget_ResultPopup->SetResultData(bIsWin, GS->AcquiredGold, GS->AcquiredExp);
				Widget_ResultPopup->SetVisibility(ESlateVisibility::Visible);

				// 팝업이 뜨면 조이스틱 입력 막기 등 처리 가능
				if (VirtualJoystick) VirtualJoystick->SetVisibility(ESlateVisibility::HitTestInvisible);
			}
		}
		break;

	default:
		break;
	}
}

void UInGameHUDWidget::OnSettingButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("Setting Button Clicked"));
}

void UInGameHUDWidget::OnAutoModeButtonClicked()
{
	// 1. 상태 토글
	bIsAutoMode = !bIsAutoMode;

	// 2. UI 텍스트/아이콘 변경 (피드백)
	if (Btn_AutoMode)
	{
		// 텍스트 변경 예시 (아이콘 교체로 해도 됨)
		FText StatusText = bIsAutoMode ? FText::FromString(TEXT("AUTO")) : FText::FromString(TEXT("MANUAL"));
		Btn_AutoMode->SetButtonText(StatusText);
	}

	// 3. 실제 게임플레이 로직에 알림 (Controller or PlayerState)
	// GetOwningPlayer()->GetComponent<UAutoCombatComponent>()->SetAutoMode(bIsAutoMode);
	UE_LOG(LogTemp, Log, TEXT("Auto Mode Toggled: %s"), bIsAutoMode ? TEXT("ON") : TEXT("OFF"));
}

void UInGameHUDWidget::OnJoystickInput(FVector2D InputVector)
{
	// 조이스틱 입력이 오면 폰(캐릭터)에게 이동 명령 전달
	if (APawn* OwnedPawn = GetOwningPlayerPawn())
	{
		const FRotator ControlRot = GetOwningPlayer()->GetControlRotation();
		const FRotator YawRot(0, ControlRot.Yaw, 0);

		const FVector ForwardDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
		const FVector RightDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

		// [수정됨] InputVector.Y에 -1.0f를 곱해서 '위로 드래그' = '전진'이 되도록 수정
		OwnedPawn->AddMovementInput(ForwardDir, InputVector.Y * -1.0f);
		OwnedPawn->AddMovementInput(RightDir, InputVector.X);
	}
}

void UInGameHUDWidget::OnUpdateHUD()
{
	// [최적화] 타이머에 의해 0.5초마다 호출됨
	AInGameGameState* GS = Cast<AInGameGameState>(UGameplayStatics::GetGameState(this));

	if (GS && GameTimerWidget && GS->bIsTimerActive)
	{
		GameTimerWidget->UpdateTime(GS->RemainingTime);
	}
}
