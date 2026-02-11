// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/Core/ParadiseGameInstance.h"
#include "Framework/System/LevelLoadingSubsystem.h"
#include "Framework/System/ParadiseSaveGame.h"
#include "Framework/InGame/InGamePlayerState.h"
#include "Components/InventoryComponent.h"
#include "Components/EquipmentComponent.h"
#include "Characters/Player/PlayerData.h"
#include "Kismet/GameplayStatics.h"


UParadiseGameInstance::UParadiseGameInstance()
{
}

void UParadiseGameInstance::Init()
{
	Super::Init();

	SaveGameSlotName = DefaultSaveSlot;

	//메인 인벤토리가 없으면 새로 생성 
	if (!MainInventory)
	{
		MainInventory = NewObject<UInventoryComponent>(this, TEXT("MainInventory"));
		//UE_LOG(LogTemp, Log, TEXT("✅ [GameInstance] 메인 인벤토리 생성 완료"));
	}

	//게임 세이브 데이터 로드 함수 호출
	LoadGameData();

	// [핵심] 서브시스템에 로딩 위젯 클래스 전달
	if (ULevelLoadingSubsystem* LoadingSystem = GetSubsystem<ULevelLoadingSubsystem>())
	{
		LoadingSystem->SetLoadingWidgetClass(LoadingWidgetClass);
	}

	UE_LOG(LogTemp, Log, TEXT("[ParadiseGameInstance] 초기화 및 로딩 서브시스템 설정 완료."));
}

void UParadiseGameInstance::SaveGameData()
{
	if (!MainInventory) return;

	//저장할 SaveGame 객체 생성
	UParadiseSaveGame* SaveObj = Cast<UParadiseSaveGame>(UGameplayStatics::CreateSaveGameObject(UParadiseSaveGame::StaticClass()));
	if (!SaveObj) return;

	//현재 메모리에 있는 인벤토리 데이터를 세이브 객체로 복사 (깊은 복사)
	SaveObj->SavedCharacters = MainInventory->GetOwnedCharacters();
	SaveObj->SavedFamiliars = MainInventory->GetOwnedFamiliars();
	SaveObj->SavedInventoryItems = MainInventory->GetOwnedItems();

	//플레이어 전체 재화 (뽑기재화 , 레벨업등에 사용하는 재화) 추가예정

	//슬롯 이름으로 디스크에 실제 파일 쓰기
	if (UGameplayStatics::SaveGameToSlot(SaveObj, SaveGameSlotName, 0))
	{
		UE_LOG(LogTemp, Log, TEXT("💾 [SaveSystem] 게임 데이터 영구 저장 완료! (슬롯: %s)"), *SaveGameSlotName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ [SaveSystem] 게임 저장에 실패했습니다."));
	}
}

void UParadiseGameInstance::LoadGameData()
{
	if (!MainInventory) return;

	//디스크에 해당 이름의 세이브 파일이 있는지 확인
	if (UGameplayStatics::DoesSaveGameExist(SaveGameSlotName, 0))
	{
		//파일이 있다면 메모리로 불러오기
		UParadiseSaveGame* LoadObj = Cast<UParadiseSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveGameSlotName, 0));
		if (LoadObj)
		{
			// 3. 세이브 객체에 들어있는 배열들을 인벤토리의 InitInventory 함수에 주입!
			// (InitInventory 함수 내부에서 자동으로 유효성 검사 후 인벤토리가 세팅됩니다)
			MainInventory->InitInventory(
				LoadObj->SavedCharacters,
				LoadObj->SavedFamiliars,
				LoadObj->SavedInventoryItems
			);

			UE_LOG(LogTemp, Log, TEXT("📂 [SaveSystem] 저장된 게임 불러오기 성공!"));
		}
	}
	else
	{
		//세이브 파일이 없다면 (처음 게임을 켰거나 데이터가 날아간 경우)
		UE_LOG(LogTemp, Warning, TEXT("📂 [SaveSystem] 세이브 파일이 없습니다. 빈 인벤토리로 시작합니다."));

		//만약 튜토리얼 기본 지급 영웅/무기가 필요하다면 여기서 AddCharacter() 등을 호출하시면 됩니다.
	}
}
