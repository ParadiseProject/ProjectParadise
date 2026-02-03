// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/BaseAttributeSet.h"
#include "GameplayEffectExtension.h"

UBaseAttributeSet::UBaseAttributeSet()
{
	// 생성자: 안전을 위해 기본값 초기화 (데이터 테이블 로딩 전 임시값)
	InitHealth(100.0f);
	InitMaxHealth(100.0f);
	InitMana(0.0f);
	InitMaxMana(0.0f);

	InitAttackPower(10.0f);
	InitDefense(0.0f);

	InitCritRate(0.0f);
	InitCritDamage(1.5f); // 기본 치명타 피해 150%

	InitMoveSpeed(400.0f);
	InitAttackSpeed(1.0f);
	InitCooldown(0.0f);
	InitAttackRange(100.0f);
}

void UBaseAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// 체력 (Health)
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	// 마나 (Mana)
	else if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
	// 치명타 확률 (CritRate)
	else if (Attribute == GetCritRateAttribute())
	{
		// 0.0(0%) ~ 1.0(100%) 사이로 제한
		NewValue = FMath::Clamp(NewValue, 0.0f, 1.0f);
	}
	// 치명타 피해 (CritDamage)
	else if (Attribute == GetCritDamageAttribute())
	{
		// 최소 1.0배(100%) 이상이어야 함 (0배면 데미지가 증발하므로)
		NewValue = FMath::Max(NewValue, 1.0f);
	}
	// 공격 속도 (AttackSpeed)
	else if (Attribute == GetAttackSpeedAttribute())
	{
		// 0.1배 미만으로 내려가면 애니메이션이 멈추거나 역재생될 수 있음
		NewValue = FMath::Max(NewValue, 0.1f);
	}
	// 사거리, 이속, 쿨타임, 공격력, 방어력 등
	else if (Attribute == GetAttackRangeAttribute() ||
		Attribute == GetMoveSpeedAttribute() ||
		Attribute == GetCooldownAttribute() ||
		Attribute == GetAttackPowerAttribute() ||
		Attribute == GetDefenseAttribute())
	{
		// 음수 방지
		NewValue = FMath::Max(NewValue, 0.0f);
	}
}

void UBaseAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float LocalDamage = GetIncomingDamage();
		SetIncomingDamage(0.f);

		if (LocalDamage > 0.f)
		{
			const float NewHealth = GetHealth() - LocalDamage;
			SetHealth(NewHealth);
		}
	}
}
