// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Data/Structs/CombatTypes.h"
#include "Data/Enums/GameEnums.h"
#include "CombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PARADISE_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	 * @brief 공격할때 필요한 데이터를 요청하는 함수
	 * @param ActionType 평타인지 스킬인지 구분 (이 값에 따라 반환되는 데미지 계수가 달라짐)
	 * @return FCombatActionData (몽타주, GE 클래스, 데미지 계수 등)
	 */
	virtual FCombatActionData GetCombatActionData(ECombatActionType ActionType) const = 0;
};
