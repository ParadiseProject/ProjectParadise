// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"


class APlayerData;
UCLASS()
class PARADISE_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ACharacterBase();

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
	/*
	 * @brief 머리위 hp위젯컴포넌트
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UWidgetComponent> HealthWidget = nullptr;


	/*
	 * @brief 데미지팝업위젯 액터 할당예정 (클래스타입도 해당 UI타입으로 변경예정)
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DamagePopupActor")
	TSubclassOf<class AActor> DamageTextActorClass = nullptr;


private:

	/*
	 * @brief 피격 이펙트 타이머 핸들
	*/
	FTimerHandle HitEffectTimerHandle;

	
	/*
	 * @brief 피격 이펙트 리셋 시간 ex) 3초후 리셋
	*/
	float HitResetTime = 3.0f;


};
