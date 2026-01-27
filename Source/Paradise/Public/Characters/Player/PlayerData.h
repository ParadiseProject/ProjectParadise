// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "AbilitySystemInterface.h"
#include "PlayerData.generated.h"

class UAttributeSet;

/**
 * 
 */
UCLASS(Blueprintable)
class PARADISE_API APlayerData : public AInfo ,public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	APlayerData();
	/*
	 * @brief ASC Getter함수
	 * @return ASC 반환
	 */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent()const override { return AbilitySystemComponent; }

	/*
	 * @brief 어트리뷰트셋 Getter함수
	 * @return 어트리뷰트셋 반환
	 */
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	/*
	 * @brief 죽으면 호출 될 함수 
	 */
	UFUNCTION()
	void OnDeath();

	/*
	 * @brief 리스폰 타이머 끝나면 호출될 함수
	 */
	UFUNCTION()
	void OnRespawnFinished();
public:

	/*
	 * @brief 플레이어 장비 컴포넌트
	 */
	UPROPERTY(VisibleAnywhere, Category = "Equipment")
	TObjectPtr<class UCMP_Equipment> EquipmentComponent = nullptr;


	/*
	 * @brief 참조중인 캐릭터 아바타
	 */
	UPROPERTY()
	TObjectPtr<class ACharacterBase> CurrentAvatar = nullptr;

	/*
	 * @brief 죽었는지 Bool값
	 */
	UPROPERTY()
	bool bIsDead =false;

	
	/*
	 * @brief 리스폰 대기시간
	 */
	UPROPERTY()
	float RespawnTimer = 5.0f;

protected:
	/*
	 * @brief 플레이어 ASC
	 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	/*
	 * @brief 실제 어트리뷰트셋 클래스로 형식 변경예정
	 */
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

private:

	/*
	 * @brief 리스폰 타이머 핸들
	 */
	FTimerHandle RespawnTimerHandle;

};
