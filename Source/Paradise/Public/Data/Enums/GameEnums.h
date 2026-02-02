#pragma once

#include "CoreMinimal.h"
#include "GameEnums.generated.h"

/**
 * @enum EUnitType
 * @brief 몬스터, 퍼밀리어의 전투 타입 (근거리, 원거리, 보스 등)
 */
UENUM(BlueprintType)
enum class EUnitType : uint8
{
	/** @brief 타입 없음 */
	None		UMETA(DisplayName = "None"),
	/** @brief 근거리 공격 타입 */
	Melee		UMETA(DisplayName = "Melee (근거리)"),
	/** @brief 원거리 공격 타입 */
	Ranged		UMETA(DisplayName = "Ranged (원거리)"),
	/** @brief 지원가 버프 타입 */
	Support		UMETA(DisplayName = "Support (지원가)"),
	/** @brief 자폭 공격 타입 */
	Siege		UMETA(DisplayName = "Siege (공성/자폭)"),
	/** @brief 보스 몬스터 타입 */
	Boss		UMETA(DisplayName = "Boss (보스)")
};

/**
 * @enum EItemRarity
 * @brief 아이템의 희귀도 등급 (Tier) 정의
 * @details 드랍 확률, UI 테두리 색상, 스탯 랜덤 보정치 계산 등에 사용되는 핵심 척도입니다.
 * 보통 1성(Common)부터 5성(Legendary)까지의 단계로 구분됩니다.
 */
	UENUM(BlueprintType)
	enum class EItemRarity : uint8
{
	/** @brief 가장 흔한 등급 (회색 테두리, 1성) */
	Common		UMETA(DisplayName = "Common (1 Star)"),

	/** @brief 일반적이지 않은 등급 (초록색 테두리, 2성) */
	Uncommon	UMETA(DisplayName = "Uncommon (2 Star)"),

	/** @brief 희귀 등급 (파란색 테두리, 3성) */
	Rare		UMETA(DisplayName = "Rare (3 Star)"),

	/** @brief 영웅 등급 (보라색 테두리, 4성) */
	Epic		UMETA(DisplayName = "Epic (4 Star)"),

	/** @brief 전설 등급 (금색 테두리, 5성) */
	Legendary	UMETA(DisplayName = "Legendary (5 Star)")
};
