// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SummonCostWidget.generated.h"

#pragma region 전방 선언
class UProgressBar;
class UTextBlock;
#pragma endregion 전방 선언

/**
 * @class USummonCostWidget
 * @brief 소환에 필요한 코스트(자원)의 상태를 실시간으로 표시하는 위젯입니다.
 * @details 오직 '표시' 역할만 수행하며, 데이터는 외부에서 주입받습니다.
 */
UCLASS()
class PARADISE_API USummonCostWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	USummonCostWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

#pragma region 외부 인터페이스
public:
	/**
	 * @brief 코스트 UI를 갱신합니다.
	 * @param CurrentCost 현재 보유량
	 * @param MaxCost 최대 보유량
	 */
	UFUNCTION(BlueprintCallable, Category = "Paradise|UI")
	void UpdateCost(float CurrentCost, float MaxCost);
#pragma endregion 외부 인터페이스

#pragma region 위젯 바인딩
private:
	/** @brief 코스트 차오름을 보여줄 진행 바 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PB_CostGauge = nullptr;

	/** @brief 수치를 텍스트로 표시 (예: 3.5 / 10) */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_CostValue = nullptr;
#pragma endregion 위젯 바인딩

#pragma region 내부 데이터
private:
	/** @brief 부드러운 게이지 차오름을 위한 캐싱 값 */
	float CachedPercent = 0.0f;
#pragma endregion 내부 데이터
};