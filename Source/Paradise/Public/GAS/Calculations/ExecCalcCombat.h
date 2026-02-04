// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalcCombat.generated.h"

/**
 * @class UExecCalcCombat
 * @brief 전투와 관련된 모든 수치 연산을 담당하는 클래스
 * @details GameplayEffect가 적용될 때 실행되며, 복잡한 수학 공식이나 태그 기반 로직을 처리합니다.
 */
UCLASS()
class PARADISE_API UExecCalcCombat : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
	UExecCalcCombat();

public:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
