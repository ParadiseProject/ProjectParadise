// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameTimerWidget.generated.h"

#pragma region 전방 선언
class UTextBlock;
class AInGameGameState;
#pragma endregion 전방 선언

/**
 * @class UGameTimerWidget
 * @brief 인게임 남은 시간을 표시하는 위젯 (뷰 전담)
 * @details 외부에서 초(Seconds)를 받아 mm:ss 포맷으로 렌더링합니다.
 */
UCLASS()
class PARADISE_API UGameTimerWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

#pragma region 외부 인터페이스
public:
	/**
	 * @brief 시간을 업데이트하고 UI를 갱신합니다.
	 * @param CurrentTime 남은 시간 (초)
	 */
	UFUNCTION(BlueprintCallable, Category = "Paradise|UI")
	void UpdateTime(int32 NewTime);
#pragma endregion 외부 인터페이스

#pragma region 내부 로직
private:
	/** @brief 초 단위를 mm:ss 문자열로 변환 */
	FText GetFormattedTime(int32 TimeInSeconds) const;
#pragma endregion 내부 로직

#pragma region 위젯 바인딩
private:
	/** @brief 시간 표시 텍스트 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_TimeValue = nullptr;
#pragma endregion 위젯 바인딩

#pragma region 데이터
private:
	/** @brief 텍스트 갱신 최적화를 위한 캐싱 값 */
	int32 CachedSeconds = -1;

	/** @brief 빨간색으로 깜빡일 기준 시간 (초) */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	int32 WarningThreshold = 10;

	UPROPERTY()
	TObjectPtr<AInGameGameState> CachedGameState;
#pragma endregion 데이터
};