// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Ingame/CharacterStatusWidget.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectTypes.h"
#include "GAS/Attributes/BaseAttributeSet.h"

void UCharacterStatusWidget::SetCharacterPortrait(UTexture2D* NewPortrait)
{
	if (Image_Portrait && NewPortrait)
	{
		Image_Portrait->SetBrushFromTexture(NewPortrait);
	}
}

void UCharacterStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();

#pragma region GAS 어트리뷰트 바인딩
	// 플레이어 폰으로부터 ASC를 안전하게 획득
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());

	if (ASC)
	{
		CachedASC = ASC;

		/** @section 어트리뷰트 변경 델리게이트 바인딩 */
		// 체력 변경 이벤트 바인딩
		ASC->GetGameplayAttributeValueChangeDelegate(UBaseAttributeSet::GetHealthAttribute())
			.AddUObject(this, &UCharacterStatusWidget::OnHealthChanged);

		// 마나 변경 이벤트 바인딩
		ASC->GetGameplayAttributeValueChangeDelegate(UBaseAttributeSet::GetManaAttribute())
			.AddUObject(this, &UCharacterStatusWidget::OnManaChanged);

		// 초기 UI 갱신 (생성 시점의 값 반영)
		const float CurrentHP = ASC->GetNumericAttribute(UBaseAttributeSet::GetHealthAttribute());
		const float MaxHP = ASC->GetNumericAttribute(UBaseAttributeSet::GetMaxHealthAttribute());
		if (PB_HealthBar && MaxHP > 0.f)
		{
			PB_HealthBar->SetPercent(CurrentHP / MaxHP);
		}

		const float CurrentMP = ASC->GetNumericAttribute(UBaseAttributeSet::GetManaAttribute());
		const float MaxMP = ASC->GetNumericAttribute(UBaseAttributeSet::GetMaxManaAttribute());
		if (PB_ManaBar && MaxMP > 0.f)
		{
			PB_ManaBar->SetPercent(CurrentMP / MaxMP);
		}
	}
#pragma endregion GAS 어트리뷰트 바인딩
}

void UCharacterStatusWidget::NativeDestruct()
{
#pragma region 메모리 및 델리게이트 정리
	// 메모리 누수 및 댕글링 포인터 방지를 위한 클린업
	if (CachedASC.IsValid())
	{
		CachedASC->GetGameplayAttributeValueChangeDelegate(UBaseAttributeSet::GetHealthAttribute()).RemoveAll(this);
		CachedASC->GetGameplayAttributeValueChangeDelegate(UBaseAttributeSet::GetManaAttribute()).RemoveAll(this);
	}

	CachedASC = nullptr;
	Super::NativeDestruct();
#pragma endregion 메모리 및 델리게이트 정리
}

#pragma region 어트리뷰트 콜백
void UCharacterStatusWidget::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	/** 
	 * @brief 체력바 업데이트 로직
	 * @details MaxHealth 값은 고정되어 있다고 가정하거나, 별도의 델리게이트로 감시할 수 있습니다.
	 */
	if (PB_HealthBar && CachedASC.IsValid())
	{
		const float MaxHP = CachedASC->GetNumericAttribute(UBaseAttributeSet::GetMaxHealthAttribute());
		if (MaxHP > 0.f)
		{
			PB_HealthBar->SetPercent(Data.NewValue / MaxHP);
		}
	}
}

void UCharacterStatusWidget::OnManaChanged(const FOnAttributeChangeData& Data)
{
	/** @brief 마나바 업데이트 로직 */
	if (PB_ManaBar && CachedASC.IsValid())
	{
		const float MaxMP = CachedASC->GetNumericAttribute(UBaseAttributeSet::GetMaxManaAttribute());
		if (MaxMP > 0.f)
		{
			PB_ManaBar->SetPercent(Data.NewValue / MaxMP);
		}
	}
}
#pragma endregion 어트리뷰트 콜백