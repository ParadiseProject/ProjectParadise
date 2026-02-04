// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/BaseGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"

UBaseGameplayAbility::UBaseGameplayAbility()
{
	// 액터마다 별도의 어빌리티 인스턴스를 생성하도록 설정
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

FGameplayEffectSpecHandle UBaseGameplayAbility::MakeTargetEffectSpec(const FGameplayEventData& EventData)
{
	// TargetEffectClass가 비어있으면 빈 핸들 반환
	if (!TargetEffectClass)
	{
		return FGameplayEffectSpecHandle();
	}

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (!SourceASC) return FGameplayEffectSpecHandle();

	// 컨텍스트 생성
	FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
	ContextHandle.AddSourceObject(this);

	// Spec 생성
	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(TargetEffectClass, GetAbilityLevel(), ContextHandle);

	return SpecHandle;
}
