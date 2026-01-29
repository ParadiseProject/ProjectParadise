#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Data/Enums/GameEnums.h"
#include "ItemStructs.generated.h"

/**
 * @struct FItemBaseStats
 * @brief 아이템의 수치(Data)와 로직적인 속성을 담당하는 최상위 부모 구조체입니다.
 * @details 모든 아이템(무기, 방어구 등)이 공통적으로 가지는 이름, 등급, 설명 등을 포함합니다.
 */
USTRUCT(BlueprintType)
struct FItemBaseStats : public FTableRowBase
{
	GENERATED_BODY()

public:
	/**
	 * @brief 인게임 UI에 표시될 아이템의 이름
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Info")
	FText DisplayName;

	/**
	 * @brief 아이템 설명 텍스트
	 * @details 툴팁이나 상세 정보창에 표시됩니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Info", meta = (MultiLine = true))
	FText Description;

	/**
	 * @brief 아이템 희귀도 (Rarity)
	 * @details Common ~ Legendary 드랍 확률 및 UI 테두리 색상 결정에 사용됩니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Info")
	EItemRarity Rarity;

	/**
	 * @brief 세트 효과 ID (Set Bonus ID)
	 * @details DT_SetBonus 테이블의 RowName과 일치해야 합니다. (세트 효과가 없으면 None)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Info")
	FName SetID;

	/**
	 * @brief 아이템 타입 (필터링 용도)
	 * @details 인벤토리 정렬이나 장착 슬롯 제한 확인에 사용됩니다. (예: Weapon, Armor)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Info")
	EItemMainType ItemType;
};

/**
 * @struct FWeaponStats
 * @brief 무기 전용 수치 데이터 구조체
 * @details FItemBaseStats를 상속받아 공격력, 사거리 등 전투에 필요한 구체적인 스탯을 정의합니다.
 */
USTRUCT(BlueprintType)
struct FWeaponStats : public FItemBaseStats
{
	GENERATED_BODY()

public:

	/**
	 * @brief 무기 스탯 생성자
	 * @details 생성 시 ItemType을 자동으로 'Weapon'으로 초기화하여, 
	 * 데이터 테이블 작성 시 타입을 일일이 지정하는 번거로움을 줄입니다.
	 */
	FWeaponStats()
	{
		ItemType = EItemMainType::Weapon;
	}

	/**
	 * @brief 무기 세부 타입
	 * @details 한손검, 활, 지팡이 등의 종류를 구분합니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Type")
	EWeaponType WeaponType;

	// -----------------------------------------------------------------
	// [Combat Stats] 전투 관련 수치
	// -----------------------------------------------------------------

	/**
	 * @brief 기본 공격력 (Base Damage)
	 * @details 무기의 순수 데미지 값입니다.
	 * GameplayEffectExecutionCalculation(ExecCalc)에서 캐릭터의 스탯(Strength)과 합산되어 최종 데미지가 계산됩니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats", meta = (ClampMin = "0.0"))
	float AttackPower;

	/**
	 * @brief 공격 속도 배율 (Attack Speed Multiplier)
	 * @details 기본값 1.0을 기준으로, 높을수록 공격 속도가 빨라집니다.
	 * 예: 1.2 = 20% 더 빠름. 애니메이션 재생 속도(PlayRate)에 곱해지거나 쿨타임 감소에 영향을 줍니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats", meta = (ClampMin = "0.1", UIMin = "0.1"))
	float AttackSpeed;

	/**
	 * @brief 치명타 확률 (Critical Rate)
	 * @details 0.0 ~ 1.0 사이의 정규화된 값을 사용합니다.
	 * 예: 0.15 = 15% 확률
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float CritRate;

	/**
	 * @brief 치명타 피해량 배율 (Critical Damage Multiplier)
	 * @details 치명타 발생 시 기본 데미지에 곱해지는 배율입니다.
	 * 예: 1.5 = 150% 데미지 (기본 100 + 추가 50). 2.0 = 200% 데미지.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats", meta = (ClampMin = "1.0"))
	float CritDamage;

	/**
	 * @brief 공격 사거리 (Attack Range)
	 * @details 단위: cm (Unreal Unit).
	 * 근거리 무기의 경우 충돌(Trace) 검사 길이로 사용됩니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats", meta = (ClampMin = "0.0"))
	float Range;

	/**
	 * @brief 스킬/공격 재사용 대기시간 (Cooldown)
	 * @details 단위: 초 (Seconds).
	 * GAS의 Cooldown GameplayEffect(GE_Cooldown)에 적용될 지속 시간(Duration)입니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats", meta = (ClampMin = "0.0"))
	float Cooldown;

	/**
	 * @brief 강화 비용 ID (Upgrade Cost ID)
	 * @details 무기 강화 시 필요한 재화 정보를 담고 있는 테이블의 RowName입니다.
	 * DT_UpgradeCost 테이블을 참조합니다. (None일 경우 강화 불가)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrade")
	FName UpgradeCostId;
};

/**
 * @struct FArmorStats
 * @brief 방어구 전용 수치 데이터 구조체
 * @details FItemBaseStats를 상속받아 최대 체력, 최대 마나, 방어력 등 생존과 관련된 구체적인 스탯을 정의합니다.
 */
USTRUCT(BlueprintType)
struct FArmorStats : public FItemBaseStats
{
	GENERATED_BODY()

public:
	/**
	* @brief 방어구 스탯 생성자
	* @details 생성 시 ItemType을 자동으로 'Armor'로 초기화하여,
	* 데이터 테이블 작성 시 타입을 일일이 지정하는 번거로움을 줄입니다.
	*/
	FArmorStats()
	{
		ItemType = EItemMainType::Armor;
	}

	/**
	* @brief 방어구 장착 부위
	* @details 투구(Helmet), 갑옷(Chest), 장갑(Gloves) 등의 부위를 결정합니다
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor Type")
	EArmorType ArmorType;

	// -----------------------------------------------------------------
	// [Survival Stats] 방어 관련 수치 
	// -----------------------------------------------------------------

	/**
	 * @brief 방어력 (DefensePower)
	 * @details 적의 공격 데미지를 감소시키는 수치입니다.
	 * GameplayEffectExecutionCalculation(ExecCalc)에서 데미지 공식의 감산(Subtraction) 혹은 비율 감소 인자로 활용됩니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival Stats", meta = (ClampMin = "0.0"))
	float DefensePower;

	/**
	 * @brief 최대 체력 보너스 (Bonus Max HP)
	 * @details 장착 시 캐릭터의 최대 체력(MaxHealth)을 증가시키는 절대값(Additive)입니다.
	 * 예: 100 입력 시 MaxHealth가 100 증가. GAS의 Passive GameplayEffect를 통해 적용됩니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival Stats")
	float MaxHP;

	/**
	 * @brief 최대 마나 보너스 (Bonus Max Mana)
	 * @details 장착 시 캐릭터의 최대 마나(MaxMana)를 증가시키는 절대값(Additive)입니다.
	 * 스킬 사용 빈도를 높여주는 옵션으로 사용됩니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival Stats")
	float MaxMana;
};