#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DT_UnitStructs.generated.h"

class USkeletalMesh;
class UAnimInstance;
class UTexture2D;
class UGameplayAbility;
class UAnimMontage;
class AAIController;
class UBehaviorTree;
class UBlackboardData;
class USoundBase;

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

/**
 * @struct FEnemyAssets
 * @brief 몬스터의 리소스(모델링, AI, 애니메이션, 사운드)를 정의하는 데이터 테이블 구조체입니다.
 * @details 밸런스 데이터(Stats)와 분리되어 있으며, 스폰 시 외형과 행동 패턴을 결정하는 데 사용됩니다.
 */
USTRUCT(BlueprintType)
struct FEnemyAssets : public FTableRowBase
{
	GENERATED_BODY()

	// =========================================================
	// 1. 외형 및 기본 설정 (Visual)
	// =========================================================

	/**
	 * @brief 몬스터 외형 모델링 (Skeletal Mesh)
	 * @details 몬스터의 뼈대가 포함된 메쉬 에셋입니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;

	/**
	 * @brief 몬스터 전용 애니메이션 블루프린트 클래스
	 * @details 해당 몬스터가 사용할 AnimInstance 클래스입니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	TSubclassOf<UAnimInstance> AnimBlueprint;

	/**
	 * @brief 몬스터 크기 배율 (Scale)
	 * @details 기본값은 1.0입니다. 보스 몬스터 등 덩치를 키워야 할 때 1.5, 2.0 등으로 설정합니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	float Scale = 1.0f; // [중요] 기본값을 1.0으로 설정하지 않으면 투명 몬스터가 될 수 있음

	// =========================================================
	// 2. 인공지능 (AI)
	// =========================================================

	/**
	 * @brief 사용할 AI 컨트롤러 클래스
	 * @details 몬스터의 두뇌 역할을 하는 컨트롤러 클래스(BP_EnemyController 등)입니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TSubclassOf<AAIController> AIController;

	/**
	 * @brief 실행할 비헤이비어 트리 에셋
	 * @details 몬스터의 행동 패턴(추적, 공격, 순찰 등)이 정의된 BT 에셋입니다. 가장 중요합니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TSoftObjectPtr<UBehaviorTree> BehaviorTree;

	/**
	 * @brief 사용할 블랙보드 데이터 에셋
	 * @details 비헤이비어 트리가 사용할 메모리(변수 저장소)입니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TSoftObjectPtr<UBlackboardData> Blackboard;

	// =========================================================
	// 3. 애니메이션 & 사운드 (Animation & Audio)
	// =========================================================

	/**
	 * @brief 공격 동작 몽타주
	 * @details 기본 공격 시 재생할 애니메이션입니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TSoftObjectPtr<UAnimMontage> AttackMontage;

	/**
	 * @brief 피격 리액션 몽타주
	 * @details 데미지를 입었을 때 재생할 경직 애니메이션입니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TSoftObjectPtr<UAnimMontage> HitMontage;

	/**
	 * @brief 사망 애니메이션 몽타주
	 * @details 체력이 0이 되었을 때 재생할 연출입니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TSoftObjectPtr<UAnimMontage> DeathMontage;

	/**
	 * @brief 사망 시 사운드 (SFX)
	 * @details 몬스터가 죽을 때 재생할 효과음(Sound Cue 또는 Wave)입니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	TSoftObjectPtr<USoundBase> DeathSound;

	// =========================================================
	// 4. GAS 어빌리티 (Abilities)
	// =========================================================

	/**
	 * @brief 스폰 시 부여할 기본 어빌리티 목록
	 * @details 몬스터가 태어날 때 ASC(AbilitySystemComponent)에 등록될 스킬들입니다. (공격, 패시브 등)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	TArray<TSubclassOf<UGameplayAbility>> AbilitySet;
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



