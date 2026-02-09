// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Data/Enums/ParadiseLobbyEnums.h"
#include "LobbyPlayerController.generated.h"

#pragma region 전방 선언
class UParadiseLobbyHUDWidget;
#pragma endregion 전방 선언

/**
 * @class ALobbyPlayerController
 * @brief 로비 전용 플레이어 컨트롤러
 */
UCLASS()
class PARADISE_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
#pragma region 로직 인터페이스
public:
	/**
	 * @brief 메뉴 변경을 요청합니다. (UI 버튼 등에서 호출)
	 * @param InNewMenu 변경할 목표 메뉴.
	 */
	UFUNCTION(BlueprintCallable, Category = "Paradise|Lobby")
	void SetLobbyMenu(EParadiseLobbyMenu InNewMenu);

	/** @brief HUD 캐싱용 (필요시 구현) */
	void SetLobbyHUD(UParadiseLobbyHUDWidget* InHUD);

private:
	/** @brief 현재 메뉴 상태 저장 */
	EParadiseLobbyMenu CurrentMenu = EParadiseLobbyMenu::None;

	UPROPERTY()
	TObjectPtr<UParadiseLobbyHUDWidget> CachedLobbyHUD;
#pragma endregion 로직 인터페이스
};