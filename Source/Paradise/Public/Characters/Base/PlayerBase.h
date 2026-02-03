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

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/*
	* @brief 플레이어가 죽은후 플레이어 데이터 액터에게 알림
	*/
	virtual void Die() override;

	/*
	 * @brief 스폰 직후 PlayerState가 이 함수를 호출해 외형등 데이터연결
	 */
	void InitializePlayer(APlayerData* InPlayerData);
	/*
	 * @brief GAS 필수 인터페이스 
	 */


protected:

	/*
	* @brief IA_Move 입력액션에 바인딩할 함수
	 * @param InValue 입력 액션의 입력값
	 */
	UFUNCTION()
	void OnMoveInput(const FInputActionValue& InValue);

	/*
	* @brief IA_Attack 입력액션에 바인딩할 함수
	 * @param InValue 입력 액션의 입력값
	 */
	UFUNCTION()
	void OnAttackInput(const FInputActionValue& InValue);

protected:

	/*
	 * @brief 입력 액션 Move
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Move = nullptr;

	/*
	 * @brief 입력 액션 Move
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Attack = nullptr;

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
	 * @details 부착될 소켓이름도 데이터 에셋에 있을예정
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FName WeaponSocketName;


	

};
