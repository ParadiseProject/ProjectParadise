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

/**
* @enum EItemMainType
* @brief 아이템의 대분류 (Category) 정의
* @details 인벤토리의 탭 구분(무기, 방어구)
*/
UENUM(BlueprintType)
	enum class EItemMainType : uint8
{
	/** @brief 무기류 (공격력 보유, WeaponStats 참조) */
	Weapon		UMETA(DisplayName = "Weapon"),

	/** @brief 방어구류 (방어력 보유, ArmorStats 참조) */
	Armor		UMETA(DisplayName = "Armor"),
};

/**
* @enum EWeaponType
* @brief 무기의 세부 종류 (Sub-Category) 정의
* @details 어떤 애니메이션 몽타주를 재생할지, 어떤 소켓에 부착할지 결정하는 핵심 Enum입니다.
*/
UENUM(BlueprintType)
	enum class EWeaponType : uint8
{
	/** @brief 한손검 (오른손 소켓) */
	Sword		UMETA(DisplayName = "One Hand Sword"),

	/** @brief 대검 (양손 소켓) */
	GreatSword	UMETA(DisplayName = "Two Hand Sword"),

	/** @brief 활 (왼손 소켓, 화살 발사체 로직 필요) */
	Bow			UMETA(DisplayName = "Bow"),

	/** @brief 마법 지팡이 (오른손 소켓, 원거리 마법 투사체) */
	Staff		UMETA(DisplayName = "Magic Staff"),

	/** @brief 단검 () */
	Dagger		UMETA(DisplayName = "Dagger")
};

/**
* @enum EArmorType
* @brief 방어구의 장착 부위 (Equip Slot) 정의
* @details 캐릭터의 Skeletal Mesh를 교체하거나(Master Pose), 덧붙일 위치를 결정합니다.
*/
UENUM(BlueprintType)
	enum class EArmorType : uint8
{
	/** @brief 머리 방어구 */
	Helmet		UMETA(DisplayName = "Helmet"),

	/** @brief 상체 갑옷 */
	Chest		UMETA(DisplayName = "Chest Armor"),

	/** @brief 장갑 */
	Gloves		UMETA(DisplayName = "Gloves"),

	/** @brief 신발 */
	Boots		UMETA(DisplayName = "Boots")
};
