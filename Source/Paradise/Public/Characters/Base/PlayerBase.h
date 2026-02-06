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

	UFUNCTION(BlueprintCallable)
	class APlayerData* GetPlayerData() const { return LinkedPlayerData.Get(); }

	/** @brief 캐릭터 사망 여부 반환 (컨트롤러 확인용) */
	UFUNCTION(BlueprintPure, Category = "Status")
	bool IsDead() const { return bIsDead; }

	/** * @brief [옵션 1] 2.5D Default
	 * @details 멀리서 망원 렌즈로 당겨 찍는 느낌. 원근 왜곡이 적어 거리 재기가 편함.
	 */
	UFUNCTION(BlueprintCallable, Category = "Camera|Preset")
	void SetCamera_Default();

	/** * @brief [옵션 2] 클래식
	 * @details 적당한 거리감과 하이 앵글. 깊이감이 잘 느껴지는 스탠다드 횡스크롤.
	 */
	UFUNCTION(BlueprintCallable, Category = "Camera|Preset")
	void SetCamera_Classic();

	/** * @brief [옵션 3] 다이나믹
	 * @details 가까운 거리, 광각 렌즈. 캐릭터 동작이 크고 시원하게 보임.
	 */
	UFUNCTION(BlueprintCallable, Category = "Camera|Preset")
	void SetCamera_Dynamic();

	/** * @brief 카메라 모드를 순차적으로 변경합니다. (0 -> 1 -> 2 -> 0 loop)
	 * @details Default -> Classic -> Dynamic 순서로 변경됩니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Camera|Control")
	void SwitchCameraMode();

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

	/** 현재 카메라 모드 인덱스 (0: Default, 1: Classic, 2: Dynamic) */
	int32 CurrentCameraIndex = 0;

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
