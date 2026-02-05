// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PARADISE_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public: 
	ALobbyGameMode();

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void StartGame();
};
