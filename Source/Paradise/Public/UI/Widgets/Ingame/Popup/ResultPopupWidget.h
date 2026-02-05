// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResultPopupWidget.generated.h"

#pragma region 전방 선언
class UTextBlock;
class UButton;
#pragma endregion 전방 선언

/**
 * @class UResultPopupWidget
 * @brief 게임 종료(승리/패배) 결과를 보여주는 팝업 위젯.
 * @details 획득한 골드/경험치를 표시하고, 로비로 돌아가거나 재시작하는 기능을 제공합니다.
 */
UCLASS()
class PARADISE_API UResultPopupWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

#pragma region 외부 인터페이스
public:
	/**
	 * @brief 결과 데이터를 설정하고 UI를 갱신합니다.
	 * @param bIsVictory 승리 여부 (True: 승리, False: 패배)
	 * @param Gold 획득 골드량
	 * @param Exp 획득 경험치량
	 */
	UFUNCTION(BlueprintCallable, Category = "Paradise|UI|Result")
	void SetResultData(bool bIsVictory, int32 Gold, int32 Exp);
#pragma endregion 외부 인터페이스

#pragma region 위젯 바인딩
private:
	/** @brief 결과 제목 (VICTORY / DEFEAT) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Title = nullptr;

	/** @brief 획득 골드 텍스트 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Gold = nullptr;

	/** @brief 획득 경험치 텍스트 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Exp = nullptr;

	/** @brief 로비로 돌아가기 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_ToLobby = nullptr;

	/** @brief 다시하기 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Retry = nullptr;
#pragma endregion 위젯 바인딩

#pragma region 내부 로직
private:
	UFUNCTION()
	void OnToLobbyClicked();

	UFUNCTION()
	void OnRetryClicked();
#pragma endregion 내부 로직
};
