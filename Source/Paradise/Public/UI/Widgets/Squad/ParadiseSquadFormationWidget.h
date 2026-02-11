// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Data/SquadUITypes.h"
#include "ParadiseSquadFormationWidget.generated.h"

#pragma region 전방 선언
class UWidgetSwitcher;
class UParadiseSquadSlot;
#pragma endregion 전방 선언

/**
 * @class UParadiseSquadFormationWidget
 * @brief 좌측 상단 편성 슬롯(메인/서브/유닛)을 관리하는 뷰 위젯
 */
UCLASS()
class PARADISE_API UParadiseSquadFormationWidget : public UUserWidget
{
	GENERATED_BODY()
	
};
