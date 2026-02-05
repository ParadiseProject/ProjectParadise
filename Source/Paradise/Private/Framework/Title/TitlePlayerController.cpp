// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/Title/TitlePlayerController.h"
#include "Blueprint/UserWidget.h"

void ATitlePlayerController::BeginPlay()
{
	Super::BeginPlay();
<<<<<<< HEAD

	// 1. 타이틀 위젯 생성 및 부착
	if (TitleHUDClass)
	{
		UUserWidget* TitleWidget = CreateWidget<UUserWidget>(this, TitleHUDClass);
		if (TitleWidget)
		{
			TitleWidget->AddToViewport();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[TitlePC] TitleHUDClass is NOT set! Please check BP_TitlePlayerController."));
	}

	// 2. 입력 모드 설정 (UI Only)
	// 캐릭터 이동은 막고, 마우스 커서는 보이게 설정합니다.
	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	SetInputMode(InputMode);
	bShowMouseCursor = true;
=======
>>>>>>> middle
}
