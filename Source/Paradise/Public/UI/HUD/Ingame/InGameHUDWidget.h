// Copyright (C) Project Paradise. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "InGameHUDWidget.generated.h"

#pragma region 전방 선언
class UActionControlPanel;
class UCharacterStatusWidget;
class UPartyStatusPanel;
class USummonControlPanel;
class UParadiseCommonButton;
#pragma endregion 전방 선언

/**
 * @class UInGameHUDWidget
 * @brief 인게임 화면의 최상위 HUD 컨테이너입니다.
 * @details Common UI의 ActivatableWidget을 상속받아 메뉴 팝업 시 입력 제어가 자동으로 처리됩니다.
 */
UCLASS()
class PARADISE_API UInGameHUDWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

#pragma region 초기화
public:
	/** 
	 * @brief 게임 시작 시 각종 패널을 초기화하고 데이터를 연결합니다. 
	 */
	UFUNCTION(BlueprintCallable, Category = "Paradise|UI")
	void InitializeHUD();
#pragma endregion 초기화

#pragma region 하위 패널 접근 (Getters)
public:
	/** @brief 액션 패널(스킬, 공격) 반환 */
	FORCEINLINE UActionControlPanel* GetActionControlPanel() const { return ActionControlPanel; }

	/** @brief 소환 패널 반환 */
	FORCEINLINE USummonControlPanel* GetSummonControlPanel() const { return SummonControlPanel; }

	/** @brief 파티 상태 패널 반환 */
	FORCEINLINE UPartyStatusPanel* GetPartyStatusPanel() const { return PartyStatusPanel; }
#pragma endregion 하위 패널 접근 (Getters)

#pragma region 내부 로직
private:
	/** @brief 설정 버튼 클릭 처리 */
	UFUNCTION()
	void OnSettingButtonClicked();

	/** @brief 자동/수동 전투 모드 전환 버튼 클릭 처리 */
	UFUNCTION()
	void OnAutoModeButtonClicked();
#pragma endregion 내부 로직

#pragma region 위젯 바인딩
private:
	/** @brief 좌측 상단 파티 목록 패널 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPartyStatusPanel> PartyStatusPanel = nullptr;

	/** @brief 우측 하단 스킬/공격 조작 패널 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UActionControlPanel> ActionControlPanel = nullptr;

	/** @brief 하단 중앙 소환수 패널 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USummonControlPanel> SummonControlPanel = nullptr;

	/** @brief 우측 상단 자동 전투 토글 버튼 (설정 버튼 왼쪽에 배치) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UParadiseCommonButton> Btn_AutoMode = nullptr;

	/** @brief 우측 상단 설정 버튼 (CommonBtn) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UParadiseCommonButton> Btn_Setting = nullptr;
#pragma endregion 위젯 바인딩

#pragma region 내부 데이터
private:
	/** @brief 현재 자동 전투 활성화 여부 */
	bool bIsAutoMode = false;
#pragma endregion 내부 데이터
};
