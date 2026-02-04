// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ActionControlPanel.generated.h"

#pragma region 전방 선언
class UCommonButtonBase;
class USkillSlotWidget;
#pragma endregion 전방 선언

/**
 * @class UActionControlPanel
 * @brief 우측 하단 전투 컨트롤러 레이아웃입니다. 개별 슬롯 및 버튼의 상태를 데이터에 따라 관리합니다.
 */
UCLASS()
class PARADISE_API UActionControlPanel : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
#pragma region 외부 인터페이스
	/**
	 * @brief 특정 스킬 슬롯의 쿨타임 데이터를 갱신합니다. (Optimization: 가상함수 방지)
	 * @param SkillIndex 0: 일반 기술, 1: 필살기
	 * @param CurrentTime 현재 남은 시간
	 * @param MaxTime 전체 쿨타임 시간
	 */
	UFUNCTION(BlueprintCallable, Category = "Paradise|UI")
	void UpdateSkillCooldown(int32 SkillIndex, float CurrentTime, float MaxTime);

	/**
	 * @brief 현재 선택된 캐릭터 인덱스에 따라 태그 버튼의 활성화 상태를 업데이트합니다.
	 * @param ActiveCharIndex 현재 플레이 중인 캐릭터 번호 (0, 1, 2)
	 */
	UFUNCTION(BlueprintCallable, Category = "Paradise|UI")
	void UpdateTagButtons(int32 ActiveCharIndex);
#pragma endregion 외부 인터페이스

private:
#pragma region 위젯 바인딩
	/** @brief 기본 공격 버튼 (Common UI) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButtonBase> AttackBtn = nullptr;

	/** @brief 액티브 스킬 슬롯 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USkillSlotWidget> SkillSlot_Active = nullptr;

	/** @brief 궁극기 스킬 슬롯 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USkillSlotWidget> SkillSlot_Ultimate = nullptr;

	/** @brief 캐릭터 교체 버튼 리스트 (배열화하여 최적화) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButtonBase> TagBtn_A = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButtonBase> TagBtn_B = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButtonBase> TagBtn_C = nullptr;
#pragma endregion 위젯 바인딩

#pragma region 내부 데이터
	/** @brief 버튼 일괄 처리를 위한 내부 캐싱 배열 */
	UPROPERTY()
	TArray<TObjectPtr<UCommonButtonBase>> TagButtons;
#pragma endregion 내부 데이터
};
