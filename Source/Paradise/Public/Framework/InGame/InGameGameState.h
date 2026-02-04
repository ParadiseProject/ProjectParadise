// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "InGameGameState.generated.h"

/**
 * 
 */
UCLASS()
class PARADISE_API AInGameGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	//스테이지 ID
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stage Data")
	FString DisplayStageName;

	//스테이지 타이머 시간(UI 타이머용)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stage Data")
	float RemainingTime;

	//스테이지 타이머 작동 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stage Data")
	bool bIsTimerActive = false;


};
