// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/BaseGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UBaseGameplayAbility::UBaseGameplayAbility()
{
	// 액터마다 별도의 어빌리티 인스턴스를 생성하도록 설정
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

ACharacter* UBaseGameplayAbility::GetPlayerCharacterFromActorInfo() const
{
	if (!CurrentActorInfo || !CurrentActorInfo->AvatarActor.IsValid())
	{
		return nullptr;
	}
	return Cast<ACharacter>(CurrentActorInfo->AvatarActor.Get());
}

AController* UBaseGameplayAbility::GetPlayerControllerFromActorInfo() const
{
	if (!CurrentActorInfo) return nullptr;
	return CurrentActorInfo->PlayerController.Get();
}

FGameplayEffectSpecHandle UBaseGameplayAbility::MakeSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, float Level)
{
	if (!EffectClass) return FGameplayEffectSpecHandle();

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (!SourceASC) return FGameplayEffectSpecHandle();

	// Context 생성: 이펙트의 출처(Instigator/Causer)를 기록
	FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
	Context.AddSourceObject(this); // 이 어빌리티가 원인임을 명시

	// Spec 생성
	return SourceASC->MakeOutgoingSpec(EffectClass, Level, Context);
}

void UBaseGameplayAbility::ApplySpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& SpecHandle)
{
	if (!TargetActor || !SpecHandle.IsValid()) return;

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (!SourceASC) return;

	// Target의 ASC를 안전하게 찾기 (Interface 혹은 Component 검색)
	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);

	if (TargetASC)
	{
		SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
	}
}