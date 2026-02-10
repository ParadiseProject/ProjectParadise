// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ParadiseLobbyTopBarWidget.generated.h"

#pragma region 전방 선언
class UTextBlock;
class UButton;
#pragma endregion 전방 선언

/**
 * @class UParadiseLobbyTopBarWidget
 * @brief 로비 상단의 재화 정보 및 시스템 메뉴(설정, 나가기)를 표시하는 위젯.
 */
UCLASS()
class PARADISE_API UParadiseLobbyTopBarWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

#pragma region UI 컴포넌트
protected:
	/** @brief 골드 표시 텍스트 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_GoldAmount = nullptr;

	/** @brief 에테르(가챠 재화) 표시 텍스트 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_AetherAmount = nullptr;

	/** @brief 설정 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Settings = nullptr;

	/** @brief 게임 종료 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_QuitGame = nullptr;
#pragma endregion UI 컴포넌트

#pragma region 내부 로직
public:
	/**
	 * @brief 재화 UI를 갱신합니다.
	 * @param InGold 현재 골드량
	 * @param InAether 현재 에테르량
	 */
	UFUNCTION(BlueprintCallable, Category = "Paradise|UI")
	void UpdateCurrencyUI(int32 InGold, int32 InAether);

private:
	UFUNCTION()
	void OnSettingsClicked();

	UFUNCTION()
	void OnQuitGameClicked();
#pragma endregion 내부 로직
};