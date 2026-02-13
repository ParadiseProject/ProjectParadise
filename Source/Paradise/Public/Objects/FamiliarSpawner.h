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
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SpawnFamiliarByID(FName UnitID);
};