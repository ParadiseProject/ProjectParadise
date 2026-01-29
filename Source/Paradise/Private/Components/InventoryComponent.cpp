// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InventoryComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UInventoryComponent::InitInventory(const TArray<FOwnedHeroData>& InHeroes, const TArray<FOwnedFamiliarData>& InFamiliars, const TArray<FOwnedItemData>& InItems)
{
}

void UInventoryComponent::AddItem(FName ItemID, int32 Count)
{
}

bool UInventoryComponent::RemoveItem(FName ItemID, int32 Count)
{
	return false;
}

void UInventoryComponent::AddHero(FName HeroID)
{
}


int32 UInventoryComponent::GetItemQuantity(FName ItemID) const
{
	return int32();
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}




