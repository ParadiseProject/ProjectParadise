// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CostManageComponent.generated.h"

/**
 * @brief 코스트 변경 시 UI에 알리기 위한 델리게이트
 * @param CurrentCost 변경된 현재 코스트
 * @param MaxCost 최대 코스트
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCostChangedDelegate, float, CurrentCost, float, MaxCost);

/**
 * @class UCostManageComponent
 * @brief 게임 내 재화(Cost)를 실시간으로 관리하는 컴포넌트
 * @details
 * - Tick을 사용하여 초당 일정량의 코스트를 자동 회복합니다.
 * - PlayerState에 부착하여 사용하기 적합합니다.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PARADISE_API UCostManageComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** @brief 생성자: 기본 변수 초기화 및 Tick 활성화 */
	UCostManageComponent();

protected:
	/** @brief 게임 시작 시 호출 (초기화 로직) */
	virtual void BeginPlay() override;

public:	
	/** * @brief 매 프레임 호출되는 함수
	 * @details bIsRegenActive가 true일 때 코스트를 회복
	 */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	/** * @brief 코스트 자동 회복을 시작합니다.
	 * @details 게임 라운드 시작 시 호출하세요.
	 */
	UFUNCTION(BlueprintCallable, Category = "Economy")
	void StartCostRegen();

	/** * @brief 코스트 자동 회복을 중단합니다.
	 * @details 게임 일시정지나 종료 시 호출하세요.
	 */
	UFUNCTION(BlueprintCallable, Category = "Economy")
	void StopCostRegen();

	/**
	 * @brief 코스트 소비 시도 (유닛 소환 등)
	 * @param Amount 소비할 금액
	 * @return 성공 시 true, 잔액 부족 시 false
	 */
	UFUNCTION(BlueprintCallable, Category = "Economy")
	bool TrySpendCost(float Amount);

	/**
	 * @brief 현재 코스트를 강제로 설정합니다. (치트/초기화용)
	 * @param NewCost 설정할 코스트 값
	 */
	UFUNCTION(BlueprintCallable, Category = "Economy")
	void SetCurrentCost(float NewCost);

public:
	/** * @brief 코스트 값이 변경될 때 호출되는 델리게이트 (UI 바인딩용)*/
	UPROPERTY(BlueprintAssignable, Category = "Economy")
	FOnCostChangedDelegate OnCostChanged;

protected:
	/** @brief 최대 코스트 보유량 (기본값: 100) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Economy")
	float MaxCost = 100.0f;

	/** * @brief 현재 보유 코스트*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Economy")
	float CurrentCost = 0.0f;

	/** @brief 초당 코스트 회복량 (기본값: 10) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Economy")
	float CostRegenRate = 10.0f;

	/** @brief 회복 활성화 여부 스위치 */
	UPROPERTY(EditDefaultsOnly, Category = "Economy")
	bool bIsRegenActive = false;
};
