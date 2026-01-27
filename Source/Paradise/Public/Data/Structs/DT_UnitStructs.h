#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DT_UnitStructs.generated.h"

class USkeletalMesh;
class UAnimInstance;
class UTexture2D;
class UGameplayAbility;
class UAnimMontage;

USTRUCT(BlueprintType)
struct FCharacterStats : public FTableRowBase
{
	GENERATED_BODY()
};

/**
 * @struct FCharacterAssetData
 * @brief 캐릭터의 리소스(에셋) 정보를 관리하는 데이터 테이블 구조체입니다.
 * @details 밸런스 데이터(Stats)와 분리되어 있으며, 메쉬, UI 아이콘, 애니메이션, 스킬 클래스 정보를 담고 있습니다.
 * 대부분의 리소스는 메모리 최적화를 위해 Soft Reference(TSoftObjectPtr)로 선언되었습니다.
 */
USTRUCT(BlueprintType)
struct FCharacterAssets : public FTableRowBase
{
	GENERATED_BODY()

public:
	// =========================================================
	// 1. 모델링 & 기본 애니메이션 (Visual & AnimBP)
	// =========================================================

	/**
	 * @brief 캐릭터 외형 (Skeletal Mesh)
	 * @details 캐릭터의 뼈대가 포함된 기본 메쉬 에셋입니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;

	/**
	 * @brief 캐릭터 전용 애니메이션 블루프린트 클래스
	 * @details 해당 캐릭터가 스폰될 때 적용할 AnimInstance 클래스입니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	TSubclassOf<UAnimInstance> AnimBlueprint;

	// =========================================================
	// 2. UI 이미지 (Icons)
	// =========================================================

	/**
	 * @brief 인게임 HUD용 얼굴 아이콘
	 * @details 전투 화면 하단이나 파티 프레임에 표시되는 작은 원형/사각형 아이콘입니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSoftObjectPtr<UTexture2D> Icon_Face;

	/**
	 * @brief 캐릭터 전신 일러스트
	 * @details 가챠 결과 화면, 캐릭터 정보창, 도감 등에서 사용되는 고해상도 이미지입니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSoftObjectPtr<UTexture2D> Icon_Body;

	// =========================================================
	// 3. GAS 어빌리티 (Gameplay Abilities)
	// =========================================================

	/**
	 * @brief 평타(기본 공격) 어빌리티 클래스
	 * @note GA_MeleeBase 등을 상속받은 구체적인 Blueprint 클래스를 할당해야 합니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	TSubclassOf<UGameplayAbility> Ability_Basic;

	/**
	 * @brief 캐릭터 고유 궁극기 어빌리티 클래스
	 * @details 쿨타임이 길고 강력한 효과를 가진 특수 스킬입니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	TSubclassOf<UGameplayAbility> Ability_Ultimate;

	// =========================================================
	// 4. 애니메이션 몽타주 (Montages)
	// =========================================================

	/**
	 * @brief 기본 공격 애니메이션 몽타주
	 * @details 연속 공격(Combo) 섹션이 포함되어 있을 수 있습니다. Ability_Basic에서 이를 재생합니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TSoftObjectPtr<UAnimMontage> Montage_Attack;

	/**
	 * @brief 궁극기 연출 몽타주
	 * @details Ability_Ultimate 사용 시 재생되며, 카메라 연출이나 슬로우 모션이 포함될 수 있습니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TSoftObjectPtr<UAnimMontage> Montage_Ultimate;

	/**
	 * @brief 피격(Hit) 리액션 몽타주
	 * @details 적에게 공격받았을 때 재생되는 경직 애니메이션입니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TSoftObjectPtr<UAnimMontage> Montage_Hit;

	/**
	 * @brief 사망(Dead) 연출 몽타주
	 * @details 체력이 0이 되었을 때 재생되며, 재생 후에는 래그돌(Ragdoll)로 전환될 수 있습니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TSoftObjectPtr<UAnimMontage> Montage_Dead;
};

USTRUCT(BlueprintType)
struct FEnemyAssets : public FTableRowBase
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FEnemyStats : public FTableRowBase
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FFamiliarAssets : public FTableRowBase
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FFamiliarStats : public FTableRowBase
{
	GENERATED_BODY()
};



