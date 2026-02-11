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
 * @class UEquipmentComponent
 * @brief 캐릭터의 장착 상태 캐싱 및 3D 외형(Visual) 업데이트를 전담하는 뷰(View) 컴포넌트
 * @details
 * - **데이터 수동화 (Data-Driven):** 스스로 장착 데이터를 조작하거나 슬롯을 판단하지 않으며, 오직 GameInstance(Inventory)로부터 전달받은 장비 맵(EquipmentMap)을 덮어써서 동기화합니다.
 * - **외형 갱신 (Visual Update):** 주입된 데이터를 바탕으로 실제 물리적 캐릭터(APlayerBase)의 소켓에 무기 액터를 스폰하여 부착하거나, 방어구 메시를 교체합니다.
 * - **빠른 조회 캐시 (Query Cache):** 전투 시스템(GAS, 애니메이션 등)이 매번 무거운 글로벌 인벤토리를 뒤지지 않도록, 현재 내 몸에 장착된 아이템의 식별자(UID/ID)를 임시로 기억하고 빠르게 반환합니다.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PARADISE_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEquipmentComponent();

	/**
	 * @brief 인게임 스폰 시, GameInstance의 장비 데이터를 받아와 외형을 초기화합니다.
	 * @details 캐릭터가 맵에 최초 스폰될 때 한 번 호출되며, 인벤토리 데이터를 바탕으로 캐시를 덮어쓰고 무기/방어구 메쉬를 생성합니다.
	 * @param InEquipmentMap GameInstance가 보관 중인 해당 캐릭터의 장착 정보 (Key: 장착 부위, Value: 아이템 GUID)
	 * @param InInventory 아이템의 실제 데이터(ID, 모델링 등)를 찾기 위해 참조할 글로벌 인벤토리 포인터
	 */
	UFUNCTION(BlueprintCallable, Category = "Equipment|Init")
	void InitializeEquipment(const TMap<EEquipmentSlot, FGuid>& InEquipmentMap, class UInventoryComponent* InInventory);

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
	 * @brief (저장용) 현재 장착 중인 모든 장비의 GUID 맵을 반환합니다.
	 */
	const TMap<EEquipmentSlot, FGuid>& GetEquippedItems() const { return EquippedItems; }

	/**
	* @brief 연결된 인벤토리 컴포넌트 반환 
	*/
	const UInventoryComponent* GetLinkedInventory() const { return LinkedInventory; }

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

private:

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
		
protected:

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
