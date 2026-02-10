// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/UnitSpawner.h"
#include "FamiliarSpawner.generated.h"

UCLASS()
class PARADISE_API AFamiliarSpawner : public AUnitSpawner
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TArray<FName> FamiliarSlots;

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SpawnFamiliarBySlot(int32 SlotIndex);
};