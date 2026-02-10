// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SummonControlPanel.generated.h"

#pragma region 전방 선언
class USummonSlotWidget;
class USummonCostWidget;
class UTexture2D;
class UCostManageComponent;
#pragma endregion 전방 선언

/**
 * @class USummonControlPanel
 * @brief 하단 중앙에 배치되는 소환수 슬롯들의 컨테이너입니다.
 * @details 배열을 사용해 슬롯을 관리하며, 확장성을 고려했습니다.
 */
UCLASS()
class PARADISE_API USummonControlPanel : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

#pragma region 내부 로직
private:
	/** @brief PlayerState가 준비될 때까지 기다렸다가 연결하는 함수 */
	void InitCostSystem();
	/**
	 * @brief 코스트 변경 델리게이트 핸들러 (직접 바인딩)
	 * @details UFUNCTION 필수
	 */
	UFUNCTION()
	void HandleCostUpdate(float CurrentCost, float MaxCost);
#pragma endregion 내부 로직

#pragma region 외부 인터페이스
public:
	/**
	 * @brief 특정 인덱스의 소환수 슬롯 데이터를 갱신합니다.
	 * @param SlotIndex 슬롯 번호 (0 ~ N)
	 * @param Icon 아이콘 텍스처
	 * @param MaxCooldown 최대 쿨타임
	 */
	UFUNCTION(BlueprintCallable, Category = "Paradise|UI")
	void SetSummonSlotData(int32 SlotIndex, UTexture2D* Icon, float MaxCooldown);

	/**
	 * @brief 특정 슬롯의 쿨타임 상태를 업데이트합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Paradise|UI")
	void UpdateSummonCooldown(int32 SlotIndex, float CurrentTime, float MaxTime);

	/** 
	 * @brief 현재 코스트 상태를 패널 내의 코스트 위젯에 전달합니다.
	 * @details Controller나 PlayerState에서 (성능상 Tick보다는 Timer가 나음) 호출하여 부드럽게 갱신할 것을 권장합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Paradise|UI")
	void UpdateCostDisplay(float CurrentCost, float MaxCost);
#pragma endregion 외부 인터페이스

#pragma region 위젯 바인딩
private:
	/** @brief 소환 코스트 표시 위젯 (상단에 배치 예정) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USummonCostWidget> CostWidget = nullptr;

	// 슬롯 위젯을 개별 바인딩합니다 (WBP에서 이름 일치 필수)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USummonSlotWidget> SummonSlot_0 = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USummonSlotWidget> SummonSlot_1 = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USummonSlotWidget> SummonSlot_2 = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USummonSlotWidget> SummonSlot_3 = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USummonSlotWidget> SummonSlot_4 = nullptr;
#pragma endregion 위젯 바인딩

#pragma region 내부 데이터
private:
	/** @brief 슬롯 위젯의 빠른 접근을 위한 캐싱 배열 */
	UPROPERTY()
	TArray<TObjectPtr<USummonSlotWidget>> SummonSlots;

	/** @brief 델리게이트 해제를 위한 컴포넌트 약참조 */
	TWeakObjectPtr<UCostManageComponent> CachedCostComponent = nullptr;

	/** @brief 재시도용 타이머 핸들 */
	FTimerHandle TimerHandle_InitCost;
#pragma endregion 내부 데이터
};