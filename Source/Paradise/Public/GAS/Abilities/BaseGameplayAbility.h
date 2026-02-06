// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "BaseGameplayAbility.generated.h"

/**
 * @class UBaseGameplayAbility
 * @brief 프로젝트의 모든 게임플레이 어빌리티(GA)가 상속받아야 하는 기본 클래스입니다.
 * * @details
 * 이 클래스는 반복적인 GAS API 호출을 줄여주는 유틸리티 함수들을 제공합니다.
 * 특히, 캐릭터가 가진 WeaponID를 통해 데이터 관리 서브시스템(Subsystem)에서
 * 실제 무기 데이터를 조회해오는 기능을 포함합니다.
 */

UCLASS()
class PARADISE_API UBaseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	/**
	 * @brief 생성자. 인스턴싱 정책(InstancingPolicy)을 설정합니다.
	 */
	UBaseGameplayAbility();

	// =========================================================================
	// Data Retrieval Helpers
	// =========================================================================

	/**
	 * @brief 현재 캐릭터가 장착 중인 무기의 전체 데이터(FWeaponAssets)를 가져옵니다.
	 * * @details
	 * 1. 실행한 캐릭터의 ICombatInterface를 통해 WeaponID(RowName)를 얻습니다.
	 * 2. UDataManagerSubsystem에 접근하여 해당 ID로 데이터 테이블을 검색합니다.
	 * 3. 검색된 데이터를 반환합니다.
	 * * @return 찾은 무기 데이터 구조체. 실패 시 빈 구조체를 반환합니다.
	 * @see ICombatInterface::GetCurrentWeaponID, UDataManagerSubsystem::GetWeaponDataByID
	 */
	//UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability|Helper")
	//FWeaponAssets GetEquippedWeaponAssets() const;

	// =========================================================================
	// Actor Helpers
	// =========================================================================

	/**
	 * @brief 어빌리티를 실행한 아바타 액터를 ACharacter로 형변환하여 반환합니다.
	 * @return Cast에 성공하면 캐릭터 포인터, 실패하면 nullptr.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability|Helper")
	ACharacter* GetPlayerCharacterFromActorInfo() const;

	/**
	 * @brief 어빌리티를 실행한 플레이어 컨트롤러를 반환합니다.
	 * @return 컨트롤러 포인터 (AI의 경우 AIController일 수 있음).
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability|Helper")
	AController* GetPlayerControllerFromActorInfo() const;

	// =========================================================================
	// Effect Helpers
	// =========================================================================

	/**
	 * @brief GameplayEffect를 적용하기 위한 스펙 핸들(SpecHandle)을 생성합니다. (택배 포장)
	 * * @param EffectClass 적용할 GE 클래스 (Blueprint Class).
	 * @param Level 어빌리티 레벨 (기본값: 1.0f). 데미지 계산식 등에서 활용됩니다.
	 * @return 생성된 FGameplayEffectSpecHandle. 실패 시 유효하지 않은 핸들 반환.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Effect")
	FGameplayEffectSpecHandle MakeSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, float Level = 1.0f);

	/**
	 * @brief 생성된 스펙 핸들을 타겟 액터에게 적용합니다. (택배 배송)
	 * * @param TargetActor 이펙트를 맞을 대상 액터.
	 * @param SpecHandle MakeSpecHandle로 생성한 핸들.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Effect")
	void ApplySpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& SpecHandle);
};