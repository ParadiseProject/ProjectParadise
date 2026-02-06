// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TitleGameMode.generated.h"

/**
 * @class ATitleGameMode
 * @brief 타이틀 레벨의 게임 모드.
 * @details 기본 플레이어 컨트롤러를 ATitlePlayerController로 교체하는 역할만 수행합니다.
 */
UCLASS()
class PARADISE_API ATitleGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ATitleGameMode();
};
