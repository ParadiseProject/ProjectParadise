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
	UEquipmentComponent* GetEquipmentComponent() const { return EquipmentComponent2; }

	/*
	 * @brief 데이터 테이블 Row 를 통해 초기화하는 함수
	 * @param InDataHandle : 데이터 테이블과 RowName이 담긴 핸들
	 */
	UFUNCTION(BlueprintCallable, Category = "Init")
	void InitStatsFromDataTable(const FDataTableRowHandle& InDataHandle);


	/**
	 * @brief FCharacterStats 구조체 데이터를 기반으로 전투 관련 어트리뷰트를 초기화합니다.
	 * * 데이터 테이블에서 로드된 기본 스탯값을 사용하여
	 * 체력(Health), 마나(Mana), 공격력, 방어력 등의 초기값을 설정합니다.
	 * * @param Stats 데이터 테이블에서 가져온 캐릭터 스탯 구조체 포인터 (유효성 검사 필요)
	 * @note MaxHealth/MaxMana 설정 후, 현재 Health/Mana도 해당 Max 값으로 즉시 동기화됩니다.
	 */
	void InitCombatAttributes(FCharacterStats* Stats);

	/** * @brief 에셋 데이터 테이블 핸들을 받아 초기화하는 함수
	 * @param InDataHandle : 데이터 테이블과 RowName이 담긴 핸들
	 * @details 테이블에서 SoftPtr을 읽어와 미리 로딩(Load)까지 수행합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Init")
	void InitAssetsFromDataTable(const FDataTableRowHandle& InAssetHandle);


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
public:
	/** * @brief 미리 로드된 스켈레탈 메시
	 * @details APlayerBase가 스폰될 때 다시 로드할 필요 없이 이 포인터를 바로 사용합니다.
	 */
	UPROPERTY(Transient, VisibleAnywhere, Category = "Cached")
	TObjectPtr<USkeletalMesh> CachedMesh = nullptr;

	/** @brief 미리 로드된 애니메이션 블루프린트 클래스 */
	UPROPERTY(Transient, VisibleAnywhere, Category = "Cached")
	TSubclassOf<UAnimInstance> CachedAnimBP = nullptr;


	/* * 장비 관리 컴포넌트
	 * @details 인게임에서 착용 중인 장비의 로직과 데이터를 처리합니다.
	 */
	UPROPERTY(VisibleAnywhere, Category = "Equipment")
	TObjectPtr<UEquipmentComponent> EquipmentComponent2 = nullptr;


	/* * 현재 빙의 중인 육체 (약한 참조)
	 * @details PlayerBase는 언제든 파괴될 수 있으므로 WeakPtr로 참조합니다.
	 */
	UPROPERTY()
	TWeakObjectPtr<class ACharacterBase> CurrentAvatar = nullptr;

	/* * @brief 캐릭터 데이터 식별자 (Data Table + Row Name)
	 * @details 에디터에서 드롭다운으로 선택 가능합니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (RowType = "CharacterStats"))
	FDataTableRowHandle CharacterStatsDataHandle;
	
	/** * @brief 캐릭터 에셋 식별자 (Data Table + Row Name)
	 * @details FCharacterAssets 구조체를 사용하는 테이블만 필터링
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (RowType = "CharacterAssets"))
	FDataTableRowHandle CharacterAssetsDataHandle;

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
	 * @brief 플레이어 ASC 컴포넌트
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;

	/* * GAS 스탯 관리용 어트리뷰트 셋
	 * @details UBaseAttributeSet 전체 스탯 관리 어트리뷰트셋
	 */
	UPROPERTY()
	TObjectPtr<UBaseAttributeSet> CombatAttributeSet = nullptr;

private:

	/*
	 * @brief 리스폰 타이머 핸들
	 */
	FTimerHandle RespawnTimerHandle;

};
