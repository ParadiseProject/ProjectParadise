// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/EquipmentComponent.h"
#include "Components/InventoryComponent.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


void UEquipmentComponent::SetLinkedInventory(UInventoryComponent* InInventory)
{

}

// Called when the game starts
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}

void UEquipmentComponent::EquipItem(FName ItemID)
{
}

void UEquipmentComponent::UnEquipItem(EEquipmentSlot Slot)
{
}

FName UEquipmentComponent::GetEquippedItemID(EEquipmentSlot Slot) const
{
	return FName();
}

void UEquipmentComponent::SetEquippedItems(const TMap<EEquipmentSlot, FName>& InItems)
{
}


void UEquipmentComponent::UpdateVisuals(APlayerBase* TargetCharacter)
{
}


void UEquipmentComponent::AttachWeaponActor(APlayerBase* Char, FName ItemID)
{
}

void UEquipmentComponent::SetArmorMesh(APlayerBase* Char, EEquipmentSlot Slot, FName ItemID)
{
}




