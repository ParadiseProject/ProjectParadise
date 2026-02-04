// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "BaseGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class PARADISE_API UBaseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UBaseGameplayAbility();

	// =================================================================
	//  설정 (Config)
	// =================================================================

	/**
	 * @brief 스킬 적중 시 타겟에게 적용할 메인 이펙트
	 * @details 데미지, 힐, 버프, 디버프 등 무엇이든 될 수 있습니다.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Effect")
	TSubclassOf<UGameplayEffect> TargetEffectClass;


	// 후에 어빌리티가 복잡해지면 변경 예정
	/**
	 * @brief 상황별로 골라 쓸 수 있는 이펙트 목록
	 * @details Key: 태그 (예: Effect.Damage, Effect.Debuff, Effect.Heal)
	 * Value: 적용할 GE 클래스
	 */
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Effect")
	//TMap<FGameplayTag, TSubclassOf<UGameplayEffect>> EffectMap;

	//// 헬퍼 함수: 태그로 특정 이펙트 스펙 만들기
	//UFUNCTION(BlueprintCallable, Category = "Ability")
	//FGameplayEffectSpecHandle MakeEffectSpecByTag(FGameplayTag EffectTag);

	// =================================================================
	//  기능 (Functions)
	// =================================================================

	/**
	 * @brief TargetEffectClass를 기반으로 SpecHandle을 생성하는 함수
	 * @details 데미지뿐만 아니라 힐, 버프 등 설정된 이펙트의 스펙을 만듭니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	FGameplayEffectSpecHandle MakeTargetEffectSpec(const FGameplayEventData& EventData);
};