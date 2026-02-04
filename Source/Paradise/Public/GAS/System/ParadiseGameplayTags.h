// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class UGameplayTagsManager;

/**
 * @brief 싱글톤 패턴으로 구현된 네이티브 게임플레이 태그 관리자
 * @details C++ 코드에서 태그를 변수처럼 안전하게 접근하기 위해 사용합니다.
 */
struct FParadiseGameplayTags
{
public:
	static const FParadiseGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeGameplayTags();

	// =========================================================
	//  여기에 필요한 태그 변수를 선언합니다.
	// =========================================================

	// [속성]
	FGameplayTag Attributes_Secondary_Armor;
	FGameplayTag Attributes_Secondary_DamageMultiplier;

	// [전투 / 상태]
	FGameplayTag Status_Debuff_Stun;
	FGameplayTag Status_Debuff_Burn;

	// [속성 / 원소]
	FGameplayTag Element_Fire;
	FGameplayTag Element_Ice;

	// [메타 / 데이터]
	FGameplayTag Data_Damage_Base; // SetByCaller용

protected:
	// 싱글톤 인스턴스
	static FParadiseGameplayTags GameplayTags;
};
