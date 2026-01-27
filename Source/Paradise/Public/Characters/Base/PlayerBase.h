// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Base/CharacterBase.h"
#include "AbilitySystemInterface.h"
#include "PlayerBase.generated.h"

/**
 * 
 */
UCLASS()
class PARADISE_API APlayerBase : public ACharacterBase, public IAbilitySystemInterface
{
	GENERATED_BODY()
	

public:
	APlayerBase();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 스폰 직후 PlayerState가 이 함수를 호출해 외형 데이터연결
	void InitializePlayer(APlayerData* InPlayerData);

	//GAS 필수 인터페이스 
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class UCameraComponent> FollowCamera;

	//클래스 타입 변경예정 //실제 데이터를 가진 액터
	UPROPERTY()
	TWeakObjectPtr<class APlayerData> LinkedPlayerData;

	// 무기 붙일 소켓 이름
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FName WeaponSocketName;

};
