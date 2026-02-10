// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Data/Structs/InventoryStruct.h" 
#include "Data/Enums/GameEnums.h"      
#include "ParadiseSaveGame.generated.h"

/**
 * @brief 게임의 영구 저장 데이터를 담는 클래스
 */
UCLASS()
class PARADISE_API UParadiseSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UParadiseSaveGame();
protected:

private:


public:
	//세이브 슬롯 이름 (기본: "SaveSlot_01")
	UPROPERTY(VisibleAnywhere, Category = "Basic")
	FString SaveSlotName;

	// 기본 슬롯 이름
	const FString DefaultSaveSlot = TEXT("SaveSlot_01");

	//유저 인덱스 (기본: 0)
	UPROPERTY(VisibleAnywhere, Category = "Basic")
	uint32 UserIndex;

	//인벤토리 데이터 (보유 아이템 목록)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TArray<FOwnedItemData> SavedInventoryItems;

	//보유 영웅 목록 (성장 정보 포함)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TArray<FOwnedCharacterData> SavedCharacter;

	//보유 패밀리어 목록
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TArray<FOwnedFamiliarData> SavedFamiliars;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TMap<FName, FCharacterEquipmentData> SavedCharacterEquipments;
};
