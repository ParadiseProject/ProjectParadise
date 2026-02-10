// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Structs/ItemStructs.h"
#include "Data/Structs/UnitStructs.h"
#include "Data/Structs/InventoryStruct.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

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
class PARADISE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

	/**
	 * @brief 계정 데이터로부터 인벤토리를 초기화하는 함수 (구조체 현재 미구현 타입변경예정)
	 * @details GameInstance(SaveFile)에 저장된 배열을 그대로 복사하여 가져옵니다.
	 * @param InHeroes 로드된 영웅 목록
	 * @param InFamiliars 로드된 퍼밀리어(병사) 목록
	 * @param InItems 로드된 장비/아이템 목록
	 */
	 UFUNCTION(BlueprintCallable, Category = "Inventory|Init")
	 void InitInventory(
		 const TArray<FOwnedCharacterData>& InHeroes,
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
	void AddItem(FName ItemID, int32 Count = 1, int32 EnhancementLvl = 0);

	/**
	 * @brief 영웅을 획득하는 함수
	 * @details 이미 보유 중인 경우, 영혼석(조각)으로 대체하거나 레벨업 재료로 변환하는 로직이 추가될 수 있습니다.
	 * @param CharacterID 획득한 영웅의 ID
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Modify")
	void AddCharacter(FName CharacterID);

	/**
	 * @brief 퍼밀리어을 획득하는 함수
	 * @param CharacterID 획득한 영웅의 ID
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Modify")
	void AddFamiliar(FName FamiliarID);

	/** * @brief 통합 삭제 함수
		* @details 인벤토리 내의 영웅, 병사, 아이템을 모두 뒤져서 해당 GUID를 가진 객체를 삭제합니다.
		* @return 성공 시 true
		*/
	UFUNCTION(BlueprintCallable, Category = "Inventory|Unified")
	bool RemoveObjectByGUID(FGuid TargetGUID, int32 Count = 1);

	/**
	 * @brief [Debug] 테스트 아이템을 추가하고 전체 목록을 출력합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void Debug_TestInventory();

	/** @return 현재 보유 중인 모든 영웅 목록 (const 참조) */
	UFUNCTION(BlueprintPure, Category = "Inventory|Query")
	const TArray<FOwnedCharacterData>& GetOwnedHeroes() const { return OwnedCharacters; }

	/** @return 현재 보유 중인 모든 퍼밀리어 목록 (const 참조) */
	UFUNCTION(BlueprintPure, Category = "Inventory|Query")
	const TArray<FOwnedFamiliarData>& GetOwnedFamiliars() const { return OwnedFamiliars; }

	/** @return 현재 보유 중인 모든 아이템 목록 (const 참조) */
	UFUNCTION(BlueprintPure, Category = "Inventory|Query")
	const TArray<FOwnedItemData>& GetOwnedItems() const { return OwnedItems; }

	/**
	 * @brief GUID로 아이템 데이터 포인터 반환 (장비 장착 시 필수)
	 * @return 찾지 못하면 nullptr
	 */
	FOwnedItemData* GetItemByGUID(FGuid TargetUID);

	/**
	 * @brief 특정 아이템의 현재 보유 개수를 반환합니다.
	 * @param ItemID 확인할 아이템 ID
	 * @return int32 보유 수량 (없으면 0)
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory|Query")
	int32 GetItemQuantity(FName ItemID) const;

	/**
	 * @brief 특정 영웅을 보유하고 있는지 확인
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory|Query")
	bool HasCharacter(FName CharacterID) const;

protected:                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
	virtual void BeginPlay() override;


private:
	/** @brief 내부 편의 함수: GameInstance 가져오기 */
	class UParadiseGameInstance* GetParadiseGI() const;


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
	TArray<FOwnedCharacterData> OwnedCharacters;

	/** [퍼밀리어] 지휘관이 보유한 병사들 (ID, Level, Quantity) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory|Storage")
	TArray<FOwnedFamiliarData> OwnedFamiliars;

	/** [아이템] 보유한 장비 및 소모품 (ID, Enhancement, Quantity) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory|Storage")
	TArray<FOwnedItemData> OwnedItems;

private:

		
};
