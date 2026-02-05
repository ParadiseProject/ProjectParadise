// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/Lobby/LobbyPlayerController.h"

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//1. 마우스 커서 보이게 설정
	bShowMouseCursor = true;

	//2. UI 전용 입력 모드 설정
	FInputModeUIOnly InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputModeData);

	UE_LOG(LogTemp, Log, TEXT("LobbyController: Mouse Cursor On"));
}
