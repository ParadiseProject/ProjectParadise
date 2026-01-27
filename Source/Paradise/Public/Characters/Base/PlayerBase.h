// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Base/CharacterBase.h"
#include "AbilitySystemInterface.h"
#include "PlayerBase.generated.h"


class UInputAction;
struct FInputActionValue;
/**
 * 
 */
UCLASS()
class PARADISE_API APlayerBase : public ACharacterBase, public IAbilitySystemInterface
{
	GENERATED_BODY()
	

public:
	APlayerBase();

	virtual void PossessedBy(AController* NewController) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	/*
	 * @brief 스폰 직후 PlayerState가 이 함수를 호출해 외형등 데이터연결
	 */
	void InitializePlayer(APlayerData* InPlayerData);
	/*
	 * @brief GAS 필수 인터페이스 
	 */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:

	/*
	 * \param InValue
	 */
	UFUNCTION()
	void OnMoveInput(const FInputActionValue& InValue);

protected:

	/*
	 * @brief 입력 액션 Move
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UInputAction> IA_Move = nullptr;

	/*
	 * @brief 스프링암 컴포넌트
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class USpringArmComponent> CameraBoom;


	/*
	 * @brief 카메라 컴포넌트
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class UCameraComponent> FollowCamera;

	
	/*
	 * @brief 클래스 타입 변경예정 ,실제 데이터를 가진 액터
	 */
	UPROPERTY()
	TWeakObjectPtr<class APlayerData> LinkedPlayerData;

	/*
	 * @brief 무기 붙일 소켓 이름
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FName WeaponSocketName;

};
