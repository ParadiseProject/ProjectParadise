#pragma once

#include "CoreMinimal.h"
#include "GameEnums.generated.h"

/**
 * @enum EUnitType
 * @brief 몬스터의 전투 타입 (근거리, 원거리, 보스 등)
 */
UENUM(BlueprintType)
enum class EUnitType : uint8
{
	None		UMETA(DisplayName = "None"),
	Melee		UMETA(DisplayName = "Melee (근거리)"),
	Ranged		UMETA(DisplayName = "Ranged (원거리)"),
	Support		UMETA(DisplayName = "Support (지원가)"),
	Boss		UMETA(DisplayName = "Boss (보스)"),
	Siege		UMETA(DisplayName = "Siege (공성/자폭)")
};

