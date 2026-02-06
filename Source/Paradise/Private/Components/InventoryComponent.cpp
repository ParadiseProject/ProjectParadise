// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InventoryComponent.h"
#include "Data/Structs/ItemStructs.h"
#include "Data/Structs/UnitStructs.h"
#include "Framework/Core/ParadiseGameInstance.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UInventoryComponent::InitInventory(const TArray<FOwnedCharacterData>& InHeroes, const TArray<FOwnedFamiliarData>& InFamiliars, const TArray<FOwnedItemData>& InItems)
{
	UParadiseGameInstance* GI = GetParadiseGI();
	if (!GI) return;

	OwnedCharacters.Empty();
	OwnedFamiliars.Empty();
	OwnedItems.Empty();

	if (InHeroes.Num() > 0)
	{
		for (int i = 0; i < InHeroes.Num(); i++)
		{
			//캐릭터(영웅) 유효성 검사
			//Character 데이터테이블 조회
			if (GI->GetDataTableRow<FCharacterAssets>(GI->CharacterAssetsDataTable, InHeroes[i].CharacterID)) {
				OwnedCharacters.Add(InHeroes[i]);
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("[UInventoryComponent] 유효성 검사 : 데이터 테이블에 존재하지 않는 아이템입니다."));
			}
		}
	}

	if (InFamiliars.Num() > 0)
	{
		for (int i = 0; i < InFamiliars.Num(); i++)
		{
			//퍼밀리어 유효성 검사
			//Familiar 데이터테이블 조회
			if (GI->GetDataTableRow<FFamiliarAssets>(GI->FamiliarAssetsDataTable, InFamiliars[i].FamiliarID)) {
				OwnedFamiliars.Add(InFamiliars[i]);
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("[UInventoryComponent] 유효성 검사 : 데이터 테이블에 존재하지 않는 아이템입니다."));
			}	
		}

	}

	if (InItems.Num() > 0)
	{
		for (int i = 0; i < InItems.Num(); i++)
		{
			//아이템 유효성 검사
			//Weapon , Armor 데이터테이블 조회
			if (GI->GetDataTableRow<FWeaponAssets>(GI->WeaponAssetsDataTable, InItems[i].ItemID)
				&& GI->GetDataTableRow<FArmorAssets>(GI->ArmorAssetsDataTable, InItems[i].ItemID)){
				OwnedItems.Add(InItems[i]);
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("[UInventoryComponent] 유효성 검사 : 데이터 테이블에 존재하지 않는 아이템입니다."));
			}
		}
	}

	/*OwnedHeroes.Append(InHeroes);
	OwnedFamiliars.Append(InFamiliars);
	OwnedItems.Append(InItems);*/

	//인벤토리 변경 델리게이트 발송
	if (OnInventoryUpdated.IsBound())
	{
		OnInventoryUpdated.Broadcast();	
	}

	UE_LOG(LogTemp, Log, TEXT("✅ 인벤토리 로드 완료 (영웅:%d, 병사:%d, 아이템:%d)"),
		OwnedCharacters.Num(), OwnedFamiliars.Num(), OwnedItems.Num());

	
}

void UInventoryComponent::AddItem(FName ItemID, int32 Count)
{
	UParadiseGameInstance* GI = GetParadiseGI();
	if (!GI) return;
	if (ItemID.IsNone()|| Count<=0) return;

	FWeaponAssets* WeaponData  = GI->GetDataTableRow<FWeaponAssets>(GI->WeaponAssetsDataTable, ItemID);
	FArmorAssets* ArmorData = GI->GetDataTableRow<FArmorAssets>(GI->ArmorAssetsDataTable, ItemID);

	if (!WeaponData&& !ArmorData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UInventoryComponent] 유효성 검사 : 데이터 테이블에 존재하지 않는 아이템입니다."));
		return;
	}

	for (int i = 0; i < OwnedItems.Num(); i++)
	{
		if (OwnedItems[i].ItemID == ItemID)
		{
			//이미 있는 아이템 //수량변동 //강화수치비교필요
			return;
		}
	}
	
	FOwnedItemData NewItem;
	NewItem.ItemID = ItemID;
	NewItem.EnhancementLevel = 1;
	NewItem.Quantity = 1;

	OwnedItems.Add(NewItem);

	//UI에 알림
	if (OnInventoryUpdated.IsBound())
	{
		OnInventoryUpdated.Broadcast();
	}
	
}

void UInventoryComponent::AddCharacter(FName CharacterID)
{
	UParadiseGameInstance* GI = GetParadiseGI();
	if (!GI) return;
	if (CharacterID.IsNone()) return;

	FCharacterAssets* CharacterData = GI->GetDataTableRow<FCharacterAssets>(GI->CharacterAssetsDataTable, CharacterID);
	if (!CharacterData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UInventoryComponent] 유효성 검사 : 데이터 테이블에 존재하지 않는 캐릭터입니다."));
		return;
	}

	for (int i = 0; i < OwnedCharacters.Num(); i++)
	{
		if (OwnedCharacters[i].CharacterID == CharacterID)
		{
			//이미 있는 캐릭터 // 조각변환등 추가처리필요
			return;
		}
	}

	//중복이 없으므로 새 캐릭터로 추가
	FOwnedCharacterData NewCharacter;
	NewCharacter.CharacterID = CharacterID;
	NewCharacter.Level = 1;
	NewCharacter.AwakeningLevel = 1;

	OwnedCharacters.Add(NewCharacter);

	//UI에 알림
	if (OnInventoryUpdated.IsBound())
	{
		OnInventoryUpdated.Broadcast();
	}
}

void UInventoryComponent::AddFamiliar(FName FamiliarID)
{
	UParadiseGameInstance* GI = GetParadiseGI();
	if (!GI) return;
	if (FamiliarID.IsNone()) return;

	FFamiliarAssets* FamiliarData = GI->GetDataTableRow<FFamiliarAssets>(GI->FamiliarAssetsDataTable, FamiliarID);

	if (!FamiliarData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UInventoryComponent] 유효성 검사 : 데이터 테이블에 존재하지 않는 퍼밀리어입니다."));
		return;
	}

	for (int i = 0; i < OwnedFamiliars.Num(); i++)
	{
		if (OwnedFamiliars[i].FamiliarID == FamiliarID)
		{
			//이미 있는 퍼밀리어 // 조각변환등 추가처리필요
			return;
		}
	}
	
	FOwnedFamiliarData NewFamiliar;
	NewFamiliar.FamiliarID = FamiliarID;
	NewFamiliar.Level = 1;
	NewFamiliar.Quantity = 1;

	OwnedFamiliars.Add(NewFamiliar);

	//UI에 알림
	if (OnInventoryUpdated.IsBound())
	{
		OnInventoryUpdated.Broadcast();
	}
}

bool UInventoryComponent::RemoveItem(FName ItemID, int32 Count)
{
	return false;
}




int32 UInventoryComponent::GetItemQuantity(FName ItemID) const
{
	return int32();
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

UParadiseGameInstance* UInventoryComponent::GetParadiseGI() const
{
	return Cast<UParadiseGameInstance>(GetWorld()->GetGameInstance());
}




