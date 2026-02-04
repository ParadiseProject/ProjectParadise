// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillSlotWidget.generated.h"

#pragma region 전방 선언
class UImage;
class UProgressBar;
class UTextBlock;
class UTexture2D;
#pragma endregion 전방 선언

/**
 * @class USkillSlotWidget
 * @brief 개별 스킬의 아이콘 표시 및 쿨타임 오버레이 로직을 전담합니다. 캐릭터 태그 시 상위 패널에 의해 데이터가 재설정됩니다.
 */
UCLASS()
class PARADISE_API USkillSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
#pragma region 데이터 업데이트
	/**
	 * @brief 캐릭터 태그 시 새로운 캐릭터의 스킬 정보로 슬롯을 초기화합니다. (Data-Driven)
	 * @param NewIcon 표시할 스킬 아이콘 텍스처
	 * @param InMaxCooldown 해당 스킬의 최대 쿨타임 수치
	 */
	UFUNCTION(BlueprintCallable, Category = "Paradise|UI")
	void UpdateSkillSlotData(UTexture2D* NewIcon, float InMaxCooldown);

	/**
	 * @brief 실시간 쿨타임 상태를 외부(Panel)로부터 전달받아 시각화합니다. (Optimization)
	 * @param CooldownTime 남은 시간 (초)
	 * @param MaxTime 스킬의 총 쿨타임 (초)
	 */
	void SetCooldownStatus(float CooldownTime, float MaxTime);
#pragma endregion 데이터 업데이트

private:
#pragma region 내부 로직 (최적화)
	/** @brief 타이머에 의해 주기적으로 호출되어 UI를 갱신하는 함수 */
	void UpdateCooldownVisual();

	/** @brief 활성화된 쿨타임 타이머를 안전하게 중지하고 핸들을 초기화합니다. */
	void StopCooldownTimer();
#pragma endregion 내부 로직 (최적화)

private:
#pragma region 쿨타임 설정
	/** 
	 * @brief 쿨타임 UI 업데이트 주기 (초)입니다.
	 * @details 값이 작을수록 숫자가 부드럽게 갱신되지만 연산 비용이 증가합니다.
	 */
	UPROPERTY(EditAnywhere, Category = "Paradise|UI|Skill", meta = (ClampMin = "0.01", UIMin = "0.01"))
	float UpdateInterval = 0.05f;
#pragma endregion 쿨타임 설정

#pragma region 위젯 바인딩
	/** @brief 스킬 아이콘 이미지 컴포넌트 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Icon = nullptr;

	/** @brief Cooldown 진행도를 표시하는 프로그레스 바 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PB_Cooldown = nullptr;

	/** @brief 남은 쿨타임 시간을 텍스트로 표시 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_CooldownTime = nullptr;
#pragma endregion 위젯 바인딩

#pragma region 캡슐화 데이터
	/** @brief 현재 스킬의 최대 쿨타임 (비율 계산용) */
	float MaxCooldownTime = 0.f;

	/** @brief 현재 남은 쿨타임 시간 */
	float CurrentCooldownTime = 0.f;

	/** @brief 쿨타임 갱신용 타이머 핸들 */
	FTimerHandle CooldownTimerHandle;
#pragma endregion 캡슐화 데이터
};
