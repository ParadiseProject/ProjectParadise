// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/Ingame/InGameHUDWidget.h"

#include "UI/Widgets/Ingame/ParadiseCommonButton.h"
#include "UI/Widgets/Ingame/VirtualJoystickWidget.h"
#include "UI/Panel/Ingame/ActionControlPanel.h"
#include "UI/Panel/Ingame/PartyStatusPanel.h"
#include "UI/Panel/Ingame/SummonControlPanel.h"
#include "UI/Widgets/Ingame/CharacterStatusWidget.h"

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

}

void UInGameHUDWidget::InitializeHUD()
{
	UE_LOG(LogTemp, Log, TEXT("InGameHUD Initialized"));
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