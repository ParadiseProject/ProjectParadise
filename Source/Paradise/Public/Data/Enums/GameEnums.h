#pragma once

#include "CoreMinimal.h"
#include "GameEnums.generated.h"

/**
 * @enum EGamePhase
 * @brief 게임 흐름 제어를 위한 상태 머신(FSM)의 주요 단계를 정의
 */
UENUM(BlueprintType)
enum class EGamePhase : uint8
{
	/** @brief [준비 상태] 전투 시작 전 카운트다운 단계  */
	Ready,	
	/** @brief [전투 상태] 메인 게임 플레이 및 스폰/타이머 활성화 */
	Combat,	
	/** @brief [승리 상태] 클리어 조건 달성 및 보상 처리 */
	Victory,
	/** @brief [패배 상태] 타임오버 또는 사망으로 인해 패배한 상태 */
	Defeat,	
	/** @brief [결과 상태] 최종 결과창을 표시하고 로비 이동을 대기하는 단계 */
	Result
};


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
