// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CMP_Equipment.h"

// Sets default values for this component's properties
UCMP_Equipment::UCMP_Equipment()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UCMP_Equipment::BeginPlay()
{
	Super::BeginPlay();

	// ...

}

void UCMP_Equipment::EquipItem(FName ItemID)
{
}

void UCMP_Equipment::UnEquipItem(EEquipmentSlot Slot)
{
}

FName UCMP_Equipment::GetEquippedItemID(EEquipmentSlot Slot) const
{
	return FName();
}

void UCMP_Equipment::SetEquippedItems(const TMap<EEquipmentSlot, FName>& InItems)
{
}


void UCMP_Equipment::UpdateVisuals(APlayerBase* TargetCharacter)
{
}


void UCMP_Equipment::AttachWeaponActor(APlayerBase* Char, FName ItemID)
{
}

void UCMP_Equipment::SetArmorMesh(APlayerBase* Char, EEquipmentSlot Slot, FName ItemID)
{
}




