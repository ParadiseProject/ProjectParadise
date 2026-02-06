// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "Engine/DataTable.h"
#include "AbilitySystemInterface.h"
#include "PlayerData.generated.h"

class UAttributeSet;
class UBaseAttributeSet;
struct FCharacterStats;
struct FCharacterAssets;
class UEquipmentComponent;

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
	 * @brief 어트리뷰트셋 (UBaseAttributeSet) Getter함수
	 * @return 어트리뷰트셋 반환
	 */
	UBaseAttributeSet* GetAttributeSet() const { return CombatAttributeSet; }

	/*
	 * @brief 장비컴포넌트 (UEquipmentComponent) Getter함수
	 * @return 장비컴포넌트 반환
	 */
	UFUNCTION(BlueprintCallable)
	UEquipmentComponent* GetEquipmentComponent() const { return EquipmentComponent2; }


	/**
	 * @brief [변경됨] 영웅 ID를 받아 GameInstance를 통해 모든 데이터를 초기화합니다.
	 * @param HeroID : 캐릭터 ID (예: "Hero_Knight")
	 */
	UFUNCTION(BlueprintCallable, Category = "Init")
	void InitPlayerData(FName HeroID);

	/*
	 * @brief 육체가 사망했을 때 호출되는 함수
	 * @details 부활 타이머를 가동하고 상태를 Dead로 변경합니다.
	 */
	UFUNCTION()
	void OnDeath();

	/*
	 * @brief 리스폰 타이머 끝나면 호출될 함수
	 */
	UFUNCTION()
	void OnRespawnFinished();

protected:
	/** @brief Combat어트리뷰트셋 데이터테이블 기반 초기화 (GI 이용)*/
	void InitCombatAttributes(FCharacterStats* Stats);
	/** @brief 플레이어 에셋 데이터테이블 기반 초기화 (GI 이용)*/
	void InitPlayerAssets(FCharacterAssets* Assets);

public:
	/** * @brief 미리 로드된 스켈레탈 메시
	 * @details APlayerBase가 스폰될 때 다시 로드할 필요 없이 이 포인터를 바로 사용합니다.
	 */
	UPROPERTY(Transient, VisibleAnywhere, Category = "Cached")
	TObjectPtr<USkeletalMesh> CachedMesh = nullptr;

	/** @brief 미리 로드된 애니메이션 블루프린트 클래스 */
	UPROPERTY(Transient, VisibleAnywhere, Category = "Cached")
	TSubclassOf<UAnimInstance> CachedAnimBP = nullptr;

	/* * 현재 빙의 중인 육체 (약한 참조)
	 * @details PlayerBase는 언제든 파괴될 수 있으므로 WeakPtr로 참조합니다.
	 */
	UPROPERTY()
	TWeakObjectPtr<class ACharacterBase> CurrentAvatar = nullptr;

	/*
	 * @brief 죽었는지 Bool값
	 */
	UPROPERTY()
	bool bIsDead =false;

	

protected:
	

	/*
	 * @brief 플레이어 ASC 컴포넌트
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;

	/* * 장비 관리 컴포넌트
	 * @details 인게임에서 착용 중인 장비의 로직과 데이터를 처리합니다.
	 */
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Equipment")
	TObjectPtr<UEquipmentComponent> EquipmentComponent2 = nullptr;

	/* * GAS 스탯 관리용 어트리뷰트 셋
	 * @details UBaseAttributeSet 전체 스탯 관리 어트리뷰트셋
	 */
	UPROPERTY()
	TObjectPtr<UBaseAttributeSet> CombatAttributeSet = nullptr;

	/*
	 * @brief 리스폰 대기시간
	 */
	UPROPERTY()
	float RespawnTimer = 5.0f;

private:

	/*
	 * @brief 리스폰 타이머 핸들
	 */
	FTimerHandle RespawnTimerHandle;

};
