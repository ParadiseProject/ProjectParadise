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
	//�������� ID
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stage Data")
	FString DisplayStageName;

	//�������� Ÿ�̸� �ð�(UI Ÿ�̸ӿ�)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stage Data")
	float RemainingTime;

	//�������� Ÿ�̸� �۵� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stage Data")
	bool bIsTimerActive = false;


};
