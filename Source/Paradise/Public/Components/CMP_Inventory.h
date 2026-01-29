// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CMP_Inventory.generated.h"


/**
 * @brief 보유 영웅 데이터 (Level, Exp, 돌파 수치 등 성장 정보 포함)
 * @details 임시 구조체
 */
USTRUCT(BlueprintType)
struct FOwnedHeroData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName HeroID; // 데이터 에셋 ID (RowName)

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AwakeningLevel = 0; // 초월/각성 단계
};

/**
 * @brief 보유 퍼밀리어(병사) 데이터
 * @details 병사는 영웅처럼 개별 성장이 있을 수도, 단순히 수량(Quantity)만 관리할 수도 있습니다.
 * 일단은 영웅과 비슷하게 ID와 레벨을 갖는 구조로 잡았습니다. 임시 구조체
 */
USTRUCT(BlueprintType)
struct FOwnedFamiliarData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName FamiliarID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Quantity = 1; // 보유 수량 (병사는 여러 마리일 수 있음)
};

/**
 * @brief 보유 장비 데이터 (강화 수치 등 포함)
 * @details 임시 구조체
 */
USTRUCT(BlueprintType)
struct FOwnedItemData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 EnhancementLevel = 0; // 강화 수치 (+1, +2...)

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Quantity = 1; // 갯수
};

/**
 * @brief 인벤토리 변경 알림 델리게이트
 * @details 아이템이나 영웅 획득/소모 시 UI 갱신을 위해 호출됩니다.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);


/**
 * @class UCMP_Inventory
 * @brief 플레이어(지휘관)의 자산(영웅, 병사, 장비)을 관리하는 컴포넌트
 * @details
 * - PlayerState에 부착되어 사용됩니다.
 * - GameInstance로부터 데이터를 받아 초기화(Init)합니다.
 * - 획득(Add) 및 소모(Remove) 시 데이터 테이블을 통해 ID 유효성을 검증합니다.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PARADISE_API UCMP_Inventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCMP_Inventory();

	/**
	 * @brief 계정 데이터로부터 인벤토리를 초기화하는 함수 (구조체 현재 미구현 타입변경예정)
	 * @details GameInstance(SaveFile)에 저장된 배열을 그대로 복사하여 가져옵니다.
	 * @param InHeroes 로드된 영웅 목록
	 * @param InFamiliars 로드된 퍼밀리어(병사) 목록
	 * @param InItems 로드된 장비/아이템 목록
	 */
	 UFUNCTION(BlueprintCallable, Category = "Inventory|Init")
	 void InitInventory(
		 const TArray<FOwnedHeroData>& InHeroes,
		 const TArray<FOwnedFamiliarData>& InFamiliars,
		 const TArray<FOwnedItemData>& InItems
	 );

	/**
	 * @brief 아이템(퍼밀리어 , 장비)을 인벤토리에 추가하는 함수
	 * @note 내부적으로 ItemDataTable을 통해 유효한 ID인지 검증합니다.
	 * @param ItemID 추가할 아이템의 RowName (ID)
	 * @param Count 추가할 개수 (기본값 1)
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Modify")
	void AddItem(FName ItemID, int32 Count = 1);

	/**
	 * @brief 아이템을 소모(삭제)하는 함수
	 * @param ItemID 소모할 아이템의 ID
	 * @param Count 소모할 개수
	 * @return bool 성공 시 true, 수량이 부족하거나 없으면 false
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Modify")
	bool RemoveItem(FName ItemID, int32 Count = 1);

	/**
	 * @brief 영웅을 획득하는 함수
	 * @details 이미 보유 중인 경우, 영혼석(조각)으로 대체하거나 레벨업 재료로 변환하는 로직이 추가될 수 있습니다.
	 * @param HeroID 획득한 영웅의 ID
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Modify")
	void AddHero(FName HeroID);

	/** @return 현재 보유 중인 모든 영웅 목록 (const 참조) */
	UFUNCTION(BlueprintPure, Category = "Inventory|Query")
	const TArray<FOwnedHeroData>& GetOwnedHeroes() const { return OwnedHeroes; }

	/** @return 현재 보유 중인 모든 퍼밀리어 목록 (const 참조) */
	UFUNCTION(BlueprintPure, Category = "Inventory|Query")
	const TArray<FOwnedFamiliarData>& GetOwnedFamiliars() const { return OwnedFamiliars; }

	/** @return 현재 보유 중인 모든 아이템 목록 (const 참조) */
	UFUNCTION(BlueprintPure, Category = "Inventory|Query")
	const TArray<FOwnedItemData>& GetOwnedItems() const { return OwnedItems; }


	/**
	 * @brief 특정 아이템의 현재 보유 개수를 반환합니다.
	 * @param ItemID 확인할 아이템 ID
	 * @return int32 보유 수량 (없으면 0)
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory|Query")
	int32 GetItemQuantity(FName ItemID) const;

protected:                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
	virtual void BeginPlay() override;

private:


public:

	/**
	 * @brief 아이템 정보가 담긴 데이터 테이블 ( 이후에 구현된 데이터 테이블로 변경예정 )
	 * @details AddItem 호출 시, 이 테이블에 존재하는 ID인지 검증하는 용도로 사용됩니다.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	TObjectPtr<UDataTable> ItemDataTable;

	/** * @brief 인벤토리 변경 시 호출되는 델리게이트 (UI)
	 */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnInventoryUpdated OnInventoryUpdated;

protected:

	/** [영웅] 지휘관이 보유한 영웅들 (ID, Level, Awakening) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory|Storage")
	TArray<FOwnedHeroData> OwnedHeroes;

	/** [퍼밀리어] 지휘관이 보유한 병사들 (ID, Level, Quantity) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory|Storage")
	TArray<FOwnedFamiliarData> OwnedFamiliars;

	/** [아이템] 보유한 장비 및 소모품 (ID, Enhancement, Quantity) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory|Storage")
	TArray<FOwnedItemData> OwnedItems;

private:

		
};
