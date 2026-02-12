// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "SummonSlotWidget.generated.h"

#pragma region 전방 선언
class UParadiseCommonButton;
class UImage;
class UProgressBar;
class UTextBlock;
class UTexture2D;
#pragma endregion 전방 선언

/** @brief 슬롯 클릭 시 인덱스를 전달하는 델리게이트 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSummonSlotClicked, int32, SlotIndex);

/**
 * @class USummonSlotWidget
 * @brief 소환수 아이콘 표시, 클릭 입력 처리 및 쿨타임 시각화를 담당하는 단일 슬롯 위젯입니다.
 */
UCLASS()
class PARADISE_API USummonSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	USummonSlotWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

#pragma region 외부 인터페이스
public:
	/**
	 * @brief 슬롯의 인덱스를 초기화합니다.
	 * @param InIndex 할당할 슬롯 번호
	 */
	void InitSlot(int32 InIndex);

	/**
	 * @brief 소환수 데이터를 설정하고 UI를 초기화합니다.
	 * @param IconTexture 표시할 소환수 아이콘 (nullptr일 경우 빈 슬롯 처리)
	 * @param InMaxCooldown 해당 소환수의 최대 쿨타임
	 * @param InCost 소환 비용
	 */
	UFUNCTION(BlueprintCallable, Category = "Paradise|UI")
	void UpdateSummonData(UTexture2D* IconTexture, float InMaxCooldown, int32 InCost);

	///**
	// * @brief 쿨타임 상태를 갱신합니다. (GAS로부터 호출 권장)
	// * @param CurrentTime 남은 시간
	// * @param MaxTime 전체 쿨타임 (재설정 필요 시)
	// * @note 26/02/12, 지금은 안쓰는 코드
	// */
	//UFUNCTION(BlueprintCallable, Category = "Paradise|UI")
	//void RefreshCooldown(float CurrentTime, float MaxTime);
#pragma endregion 외부 인터페이스

#pragma region 내부 로직
private:
	/** @brief 버튼 클릭 시 발생할 이벤트 핸들러 */
	UFUNCTION()
	void OnSummonButtonClicked();

	///**
	// * @brief 타이머에 의해 주기적으로 호출되어 쿨타임 UI를 갱신합니다.
	// * @note 26/02/12, 지금은 안쓰는 코드
	// */
	//UFUNCTION()
	//void UpdateCooldownVisual();

	///** @brief 쿨타임 UI를 숨기고 상태를 리셋합니다. */
	//UFUNCTION()
	//void StopCooldownTimer();
#pragma endregion 내부 로직

#pragma region 쿨타임 설정
private:
	/** @brief 쿨타임 UI 업데이트 주기 (초)입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Paradise|UI", meta = (AllowPrivateAccess = "true", ClampMin = "0.01"))
	float UpdateInterval = 0.05f;
#pragma endregion 쿨타임 설정

#pragma region 위젯 바인딩
private:
	/** @brief 클릭 입력을 담당하는 Common UI 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UParadiseCommonButton> Btn_SummonAction = nullptr;

	/** @brief 소환수 아이콘 이미지 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_SummonIcon = nullptr;

	///** @brief 쿨타임 진행바 */
	///  * @note 26/02/12, 지금은 안쓰는 코드
	//UPROPERTY(meta = (BindWidget))
	//TObjectPtr<UProgressBar> PB_Cooldown = nullptr;

	///** @brief 남은 시간을 표시할 텍스트 */
	///  * @note 26/02/12, 지금은 안쓰는 코드
	//UPROPERTY(meta = (BindWidget))
	//TObjectPtr<UTextBlock> Text_CooldownTime = nullptr;

	/** @brief 유닛의 코스트를 표시할 텍스트 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_CostValue = nullptr;
#pragma endregion 위젯 바인딩

#pragma region 데이터
public:
	/** @brief 슬롯 클릭 알림 델리게이트 */
	UPROPERTY(BlueprintAssignable, Category = "Paradise|Event")
	FOnSummonSlotClicked OnSlotClicked;

private:
	/** @brief 이 슬롯의 고유 인덱스 (0~4) */
	int32 SlotIndex = -1;

	/** @brief 최대 쿨타임 변수 */
	UPROPERTY()
	float MaxCooldownTime = 0.0f;

	/** @brief 현재 쿨타임 변수 */
	UPROPERTY()
	float CurrentCooldownTime = 0.0f;

	/** @brief 쿨타임  */
	UPROPERTY()
	FTimerHandle CooldownTimerHandle;
#pragma endregion 데이터
};
