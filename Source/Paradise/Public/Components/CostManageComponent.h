// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CostManageComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCostChangedDelegate, float, CurrentCost, float, MaxCost);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PARADISE_API UCostManageComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCostManageComponent();

protected:
	virtual void BeginPlay() override;

public:	
	// 매 프레임 코스트 회복
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
public:
	UFUNCTION(BlueprintCallable, Category = "Economy")
	void StartCostRegen();

	UFUNCTION(BlueprintCallable, Category = "Economy")
	void StopCostRegen();

	/**
	 * @brief 코스트 소비 시도 (유닛 소환 등)
	 * @param Amount 소비할 금액
	 * @return 성공 시 true, 잔액 부족 시 false
	 */
	UFUNCTION(BlueprintCallable, Category = "Economy")
	bool TrySpendCost(float Amount);

public:
	UPROPERTY(BlueprintAssignable, Category = "Economy")
	FOnCostChangedDelegate OnCostChanged;

protected:
	//최대 코스트 
	UPROPERTY(EditDefaultsOnly, Category = "Economy")
	float MaxCost = 100.0f;

	//초당 회복량
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Economy")
	float CostRegenRate = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Economy")
	bool bIsRegenActive = false;

	//현재 코스트
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_CurrentCost, Category = "Economy")
	float CurrentCost = 0.0f;

	//클라이언트에서 값이 도착하면 호출되는 함수
	UFUNCTION()
	void OnRep_CurrentCost();



		
};
