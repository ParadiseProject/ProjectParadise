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
	// 현재 스테이지 표시 이름 (예: "1-1", "Boss Room")
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stage Data")
	FString DisplayStageName;

	// 남은 스테이지 시간 (UI 타이머 표시용)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stage Data")
	float RemainingTime;

	// 타이머 작동 여부 (일시정지 등 확인)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stage Data")
	bool bIsTimerActive = false;


};
