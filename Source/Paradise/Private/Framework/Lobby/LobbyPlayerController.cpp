// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/Lobby/LobbyPlayerController.h"
#include "UI/HUD/Lobby/ParadiseLobbyHUDWidget.h"

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

void ALobbyPlayerController::SetLobbyHUD(UParadiseLobbyHUDWidget* InHUD)
{
    CachedLobbyHUD = InHUD;
}

void ALobbyPlayerController::SetLobbyMenu(EParadiseLobbyMenu InNewMenu)
{
    if (CurrentMenu == InNewMenu) return;

    CurrentMenu = InNewMenu;
    UE_LOG(LogTemp, Log, TEXT("🎮 [Controller] 메뉴 변경: %d"), (int32)CurrentMenu);

    // HUD에게 UI 변경 지시
    if (CachedLobbyHUD)
    {
        CachedLobbyHUD->UpdateMenuStats(CurrentMenu);
    }
}