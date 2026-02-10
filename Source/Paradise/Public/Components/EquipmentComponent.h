// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enums/GameEnums.h"
#include "Data/Structs/ItemStructs.h"
#include "Data/Structs/InventoryStruct.h"
#include "Components/ActorComponent.h"
#include "EquipmentComponent.generated.h"

class UDataTable;
class APlayerBase;
class UInventoryComponent;


/**
 * @brief 장비 상태 변경 알림 델리게이트
 * @details 장착/해제로 인해 장비 상태가 변했을 때 UI 갱신 등을 위해 호출됩니다.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipmentUpdated);

/**
 * @class UCMP_Equipment
 * @brief 캐릭터(영혼/PlayerData)의 장비 착용 상태 및 외형 동기화를 관리하는 컴포넌트
 * @details
 * - **스마트 장착:** ItemID만 입력하면 데이터 테이블을 조회해 자동으로 슬롯을 찾아 장착합니다.
 * - **외형 동기화:** 장착된 정보를 바탕으로 PlayerBase의 무기를 스폰하거나 방어구 메시를 교체합니다.
 * - **상태 보존:** PlayerData에 부착되어 있으므로, 육체가 파괴되거나 교체되어도 장비 상태는 유지됩니다.
 * - **[참고]** 현재 코드에서 사용되는 `EEquipmentSlot` Enum 타입은 아직 정식 구현 전 단계이며, 임시로 정의된 상태입니다.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PARADISE_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEquipmentComponent();

	/**
	 * @brief [통합 테스트] 인벤토리 지급 -> GUID 조회 -> 장착 -> 결과 확인
	 * @details 무기(Iron_Sword)와 방어구(Leather_Chest)를 임의로 생성해 장착까지 시도합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void Debug_TestEquipmentSystem();

	/**
	 * @brief 아이템 ID만으로 장비를 장착하는 스마트 함수
	 * @details
	 * 1. ItemDataTable에서 ItemID를 검색하여 EEquipmentSlot(장착 부위)을 알아냅니다.
	 * 2. 해당 슬롯에 기존 장비가 있다면 해제(UnEquip) 후 교체합니다.
	 * 3. EquippedItems 맵을 갱신하고, 현재 빙의된 육체가 있다면 즉시 UpdateVisuals를 호출합니다.
	 * @param ItemID 장착할 아이템의 ID (RowName)
	 */
	UFUNCTION(BlueprintCallable, Category = "Equipment|Modify")
	void EquipItem(FGuid TargetItemUID);

	/**
	 * @brief 특정 슬롯의 장비를 해제합니다.
	 * @param Slot 해제할 장비 슬롯 (Weapon, Helmet 등)
	 */
	UFUNCTION(BlueprintCallable, Category = "Equipment|Modify")
	void UnEquipItem(EEquipmentSlot Slot);

	/**
	 * @brief 현재 특정 슬롯에 장착된 아이템 ID를 반환합니다.
	 * @return 아이템 ID (장착된 게 없으면 None)
	 */
	UFUNCTION(BlueprintPure, Category = "Equipment|Query")
	FName GetEquippedItemID(EEquipmentSlot Slot) const;

	/**
	 * @brief 현재 슬롯에 장착된 아이템의 상세 데이터(강화수치, 수량 등)를 반환합니다.
	 * @return 데이터 찾음 여부
	 */
	UFUNCTION(BlueprintPure, Category = "Equipment|Query")
	bool GetEquippedItemData(EEquipmentSlot Slot, FOwnedItemData& OutData) const;

	/**
	 * @brief (저장용) 현재 장착 중인 모든 장비 상태를 반환합니다.
	 * @details GameInstance나 SaveGame에 저장할 때 사용합니다.
	 */
	/*const TMap<EEquipmentSlot, FName>& GetEquippedItems() const { return EquippedItems; }*/

	/**
	 * @brief (저장용) 현재 장착 중인 모든 장비의 GUID 맵을 반환합니다.
	 */
	const TMap<EEquipmentSlot, FGuid>& GetEquippedItems() const { return EquippedItems; }

	/**
	 * @brief 현재 장비 상태에 맞춰 대상 캐릭터(육체)의 외형을 갱신합니다.
	 * @details
	 * - PlayerBase가 스폰되거나 빙의될 때(InitializePlayer) 호출됩니다.
	 * - 장비가 변경될 때도 내부적으로 호출됩니다.
	 * @param TargetCharacter 외형을 적용할 실제 캐릭터 액터
	 */
	UFUNCTION(BlueprintCallable, Category = "Equipment|Visual")
	void UpdateVisuals(APlayerBase* TargetCharacter);

	/* * @brief 소유 확인을 위해 인벤토리 컴포넌트 참조 설정
	 * @details PlayerData 생성 시점에 주입해줍니다.
	 */
	void SetLinkedInventory(UInventoryComponent* InInventory);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:

	/**
	 * @brief 아이템 ID를 기반으로 장착되어야 할 슬롯을 찾습니다.
	 * @details 무기/방어구 테이블을 조회하고 태그를 비교합니다.
	 */
	EEquipmentSlot FindEquipmentSlot(FName ItemID) const;

	/**
	 * @brief (내부함수) 무기 액터를 스폰하고 캐릭터 소켓에 부착합니다.
	 * @details 기존 무기가 있다면 파괴하고 새 무기를 생성합니다.
	 */
	void AttachWeaponActor(APlayerBase* Char, FName ItemID);

	/**
	 * @brief (내부함수) 캐릭터의 스켈레탈 메시(방어구)를 교체합니다.
	 * @details 투구, 갑옷, 신발 등 부위별로 메시를 SetSkeletalMesh 합니다.
	 */
	void SetArmorMesh(APlayerBase* Char, EEquipmentSlot Slot, FName ItemID);


public:

	/** * @brief 장비 변경 시 호출되는 이벤트 (UI 갱신용)
	 */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEquipmentUpdated OnEquipmentUpdated;
		
protected:

	/**
	 * @brief 현재 장착 중인 아이템 목록
	 * @details [Key: 슬롯 타입] -> [Value: 아이템 ID]
	 * @warning 사용되는 EEquipmentSlot Enum 타입은 현재 임시 구현 상태입니다.
	 */
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment|State")
	TMap<EEquipmentSlot, FName> EquippedItems;*/

	/**
	 * @brief [핵심 변경] 현재 장착 중인 아이템 목록
	 * @details Key: 장착 슬롯 / Value: 아이템의 고유 식별자 (GUID)
	 * 기존 FName 대신 FGuid를 저장하여 특정 아이템(강화된 것)을 정확히 추적합니다.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment|State")
	TMap<EEquipmentSlot, FGuid> EquippedItems;
		
	UPROPERTY()
	TObjectPtr<UInventoryComponent> LinkedInventory = nullptr; // 소유권 확인용



private:
	/** 현재 스폰된 무기 액터 (관리용) */
	UPROPERTY()
	TObjectPtr<AActor> SpawnedWeaponActor = nullptr;
};
