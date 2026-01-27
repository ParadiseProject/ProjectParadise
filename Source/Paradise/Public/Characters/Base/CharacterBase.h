// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "CharacterBase.generated.h"

UCLASS()
class PARADISE_API ACharacterBase : public ACharacter,public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACharacterBase();

	// 스폰 직후 PlayerState가 이 함수를 호출해 외형 데이터연결
	//void InitializeHero(AHeroDataActor* InHeroData);

	//GAS 필수 인터페이스 
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void BeginPlay() override;

	virtual float TakeDamage(
		float DamageAmount,
		FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;


	/*
	 * @brief Hit 이펙트를 적용 시키는 함수 (마테리얼 변화)
	 */
	UFUNCTION(BlueprintCallable)
	void PlayHitFlash();


	/*
	 * @brief Hit 이펙트를 리셋 시키는 함수 (마테리얼 변화)
	*/
	UFUNCTION(BlueprintCallable)
	void ResetHitFlash();


	/* 
	 * @brief DamagePopup(데미지위젯)을 스폰(오브젝트 풀링)시키는 함수  
	 * @param DamageAmount 위젯에 설정할 데미지수치
	 */
	UFUNCTION(BlueprintCallable)
	void SpawnDamagePopup(float DamageAmount);

public:	

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;



protected:
	//머리위 hp위젯
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UWidgetComponent> HealthWidget = nullptr;

	//데미지팝업위젯 액터 할당예정 (클래스타입도 해당 UI타입으로 변경예정)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DamagePopupActor")
	TSubclassOf<class AActor> DamageTextActorClass = nullptr;

	//클래스 타입 변경예정 //실제 데이터를 가진 액터
	UPROPERTY()
	TWeakObjectPtr<class AActor> LinkedHeroData;

	// 무기 붙일 소켓 이름
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FName WeaponSocketName;

private:

	//피격 이펙트 타이머 핸들
	FTimerHandle HitEffectTimerHandle;

	//피격 이펙트 리셋 시간 ex) 3초후 리셋
	float HitResetTime = 3.0f;


};
