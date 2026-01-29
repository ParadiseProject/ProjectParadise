// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CMP_Inventory.h"

// Sets default values for this component's properties
UCMP_Inventory::UCMP_Inventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UCMP_Inventory::InitInventory(const TArray<FOwnedHeroData>& InHeroes, const TArray<FOwnedFamiliarData>& InFamiliars, const TArray<FOwnedItemData>& InItems)
{
}

void UCMP_Inventory::AddItem(FName ItemID, int32 Count)
{
}

bool UCMP_Inventory::RemoveItem(FName ItemID, int32 Count)
{
	return false;
}

void UCMP_Inventory::AddHero(FName HeroID)
{
}


int32 UCMP_Inventory::GetItemQuantity(FName ItemID) const
{
	return int32();
}

// Called when the game starts
void UCMP_Inventory::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}




