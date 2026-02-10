// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InventoryComponent.h"
#include "Framework/Core/ParadiseGameInstance.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::InitInventory(const TArray<FOwnedCharacterData>& InHeroes, const TArray<FOwnedFamiliarData>& InFamiliars, const TArray<FOwnedItemData>& InItems)
{
	UParadiseGameInstance* GI = GetParadiseGI();
	if (!GI) return;

	OwnedCharacters.Empty();
	OwnedFamiliars.Empty();
	OwnedItems.Empty();

	// 1. 영웅(Character) 로드
	if (InHeroes.Num() > 0)
	{
		for (int i = 0; i < InHeroes.Num(); i++)
		{
			// [유효성 검사] Assets 테이블과 Stats 테이블 모두 존재해야 함
			bool bHasAsset = (GI->GetDataTableRow<FCharacterAssets>(GI->CharacterAssetsDataTable, InHeroes[i].CharacterID) != nullptr);
			bool bHasStat = (GI->GetDataTableRow<FCharacterStats>(GI->CharacterStatsDataTable, InHeroes[i].CharacterID) != nullptr);

			if (bHasAsset && bHasStat)
			{
				OwnedCharacters.Add(InHeroes[i]);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[Inventory] 영웅 유효성 실패(Asset/Stat 누락): %s"), *InHeroes[i].CharacterID.ToString());
			}
		}
	}

	// 2. 퍼밀리어(Familiar) 로드
	if (InFamiliars.Num() > 0)
	{
		for (int i = 0; i < InFamiliars.Num(); i++)
		{
			// [유효성 검사] Assets 테이블과 Stats 테이블 모두 존재해야 함
			bool bHasAsset = (GI->GetDataTableRow<FFamiliarAssets>(GI->FamiliarAssetsDataTable, InFamiliars[i].FamiliarID) != nullptr);
			bool bHasStat = (GI->GetDataTableRow<FFamiliarStats>(GI->FamiliarStatsDataTable, InFamiliars[i].FamiliarID) != nullptr);

			if (bHasAsset && bHasStat)
			{
				OwnedFamiliars.Add(InFamiliars[i]);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[Inventory] 퍼밀리어 유효성 실패(Asset/Stat 누락): %s"), *InFamiliars[i].FamiliarID.ToString());
			}
		}
	}

	// 3. 아이템(Item) 로드 (무기 or 방어구)
	if (InItems.Num() > 0)
	{
		for (int i = 0; i < InItems.Num(); i++)
		{
			FName ID = InItems[i].ItemID;

			// A. 무기(Weapon) 검증: Assets & Stats 모두 존재
			bool bIsValidWeapon = (GI->GetDataTableRow<FWeaponAssets>(GI->WeaponAssetsDataTable, ID) != nullptr)
				&& (GI->GetDataTableRow<FWeaponStats>(GI->WeaponStatsDataTable, ID) != nullptr);

			// B. 방어구(Armor) 검증: Assets & Stats 모두 존재
			bool bIsValidArmor = (GI->GetDataTableRow<FArmorAssets>(GI->ArmorAssetsDataTable, ID) != nullptr)
				&& (GI->GetDataTableRow<FArmorStats>(GI->ArmorStatsDataTable, ID) != nullptr);

			// 둘 중 하나라도 완벽하게 존재하면 추가
			if (bIsValidWeapon || bIsValidArmor)
			{
				OwnedItems.Add(InItems[i]);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[Inventory] 아이템 유효성 실패(Asset/Stat 누락): %s"), *ID.ToString());
			}
		}
	}

	if (OnInventoryUpdated.IsBound())
	{
		OnInventoryUpdated.Broadcast();
	}

	UE_LOG(LogTemp, Log, TEXT("✅ 인벤토리 로드 완료 (영웅:%d, 병사:%d, 아이템:%d)"),
		OwnedCharacters.Num(), OwnedFamiliars.Num(), OwnedItems.Num());
}

void UInventoryComponent::AddItem(FName ItemID, int32 Count, int32 EnhancementLvl)
{
	UParadiseGameInstance* GI = GetParadiseGI();
	if (!GI) return;
	if (ItemID.IsNone() || Count <= 0) return;

	// 1. 무기(Weapon) 유효성 검사 (Assets && Stats)
	bool bIsValidWeapon = (GI->GetDataTableRow<FWeaponAssets>(GI->WeaponAssetsDataTable, ItemID) != nullptr)
		&& (GI->GetDataTableRow<FWeaponStats>(GI->WeaponStatsDataTable, ItemID) != nullptr);

	// 2. 방어구(Armor) 유효성 검사 (Assets && Stats)
	bool bIsValidArmor = (GI->GetDataTableRow<FArmorAssets>(GI->ArmorAssetsDataTable, ItemID) != nullptr)
		&& (GI->GetDataTableRow<FArmorStats>(GI->ArmorStatsDataTable, ItemID) != nullptr);

	// 둘 다 아니면 실패
	if (!bIsValidWeapon && !bIsValidArmor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AddItem] 실패: ID(%s)가 무기/방어구 테이블(Assets & Stats) 쌍에 존재하지 않습니다."), *ItemID.ToString());
		return;
	}

	for (int32 i = 0; i < Count; i++)
	{
		FOwnedItemData NewItem;
		NewItem.ItemUID = FGuid::NewGuid(); // ⭐ 핵심: 고유 ID 발급
		NewItem.ItemID = ItemID;
		NewItem.EnhancementLevel = EnhancementLvl;
		NewItem.Quantity = 1;

		OwnedItems.Add(NewItem);

		UE_LOG(LogTemp, Log, TEXT("✨ [AddItem] 장비 개별 획득: %s (UID: %s)"),
			*ItemID.ToString(), *NewItem.ItemUID.ToString());
	}

	if (OnInventoryUpdated.IsBound()) OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::AddCharacter(FName CharacterID)
{
	UParadiseGameInstance* GI = GetParadiseGI();
	if (!GI) return;
	if (CharacterID.IsNone()) return;

	// [유효성 검사] CharacterAssets와 CharacterStats 모두 조회
	bool bHasAsset = (GI->GetDataTableRow<FCharacterAssets>(GI->CharacterAssetsDataTable, CharacterID) != nullptr);
	bool bHasStat = (GI->GetDataTableRow<FCharacterStats>(GI->CharacterStatsDataTable, CharacterID) != nullptr);

	if (!bHasAsset || !bHasStat)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AddCharacter] 실패: ID(%s)가 캐릭터 Assets 혹은 Stats 테이블에 없습니다."), *CharacterID.ToString());
		return;
	}

	for (int i = 0; i < OwnedCharacters.Num(); i++)
	{
		if (OwnedCharacters[i].CharacterID == CharacterID)
		{
			// 이미 보유 중
			return;
		}
	}

	FOwnedCharacterData NewCharacter;
	NewCharacter.CharacterUID = FGuid::NewGuid();
	NewCharacter.CharacterID = CharacterID;
	NewCharacter.Level = 1;
	NewCharacter.AwakeningLevel = 1;

	OwnedCharacters.Add(NewCharacter);

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

	// [유효성 검사] FamiliarAssets와 FamiliarStats 모두 조회
	bool bHasAsset = (GI->GetDataTableRow<FFamiliarAssets>(GI->FamiliarAssetsDataTable, FamiliarID) != nullptr);
	bool bHasStat = (GI->GetDataTableRow<FFamiliarStats>(GI->FamiliarStatsDataTable, FamiliarID) != nullptr);

	if (!bHasAsset || !bHasStat)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AddFamiliar] 실패: ID(%s)가 퍼밀리어 Assets 혹은 Stats 테이블에 없습니다."), *FamiliarID.ToString());
		return;
	}

	// [수정 후] 무조건 새로운 병사로 추가
	FOwnedFamiliarData NewFamiliar;
	NewFamiliar.FamiliarUID = FGuid::NewGuid(); // ⭐ 핵심
	NewFamiliar.FamiliarID = FamiliarID;
	NewFamiliar.Level = 1;
	NewFamiliar.Quantity = 1;

	OwnedFamiliars.Add(NewFamiliar);

	UE_LOG(LogTemp, Log, TEXT("🥚 [AddFamiliar] 병사 영입: %s (UID: %s)"),
		*FamiliarID.ToString(), *NewFamiliar.FamiliarUID.ToString());

	if (OnInventoryUpdated.IsBound()) OnInventoryUpdated.Broadcast();
}

bool UInventoryComponent::RemoveObjectByGUID(FGuid TargetGUID, int32 Count)
{
	if (!TargetGUID.IsValid()) return false;

	// 1. [아이템] 배열 검색
	for (int32 i = 0; i < OwnedItems.Num(); ++i)
	{
		if (OwnedItems[i].ItemUID == TargetGUID)
		{
			// 장비 로직: 수량 차감
			if (OwnedItems[i].Quantity > Count)
			{
				OwnedItems[i].Quantity -= Count;
				UE_LOG(LogTemp, Log, TEXT("📉 아이템 수량 감소: %s"), *TargetGUID.ToString());
			}
			else
			{
				OwnedItems.RemoveAt(i);
				UE_LOG(LogTemp, Log, TEXT("🗑️ 아이템 삭제 완료: %s"), *TargetGUID.ToString());
			}

			if (OnInventoryUpdated.IsBound()) OnInventoryUpdated.Broadcast();
			return true; // 찾았으니 리턴
		}
	}

	// 2. [퍼밀리어] 배열 검색
	for (int32 i = 0; i < OwnedFamiliars.Num(); ++i)
	{
		if (OwnedFamiliars[i].FamiliarUID == TargetGUID)
		{
			// 병사는 개별 관리이므로 즉시 삭제
			OwnedFamiliars.RemoveAt(i);
			UE_LOG(LogTemp, Log, TEXT("🗑️ 퍼밀리어 삭제 완료: %s"), *TargetGUID.ToString());

			if (OnInventoryUpdated.IsBound()) OnInventoryUpdated.Broadcast();
			return true;
		}
	}

	// 3. [영웅] 배열 검색
	for (int32 i = 0; i < OwnedCharacters.Num(); ++i)
	{
		if (OwnedCharacters[i].CharacterUID == TargetGUID)
		{
			OwnedCharacters.RemoveAt(i);
			UE_LOG(LogTemp, Log, TEXT("👋 영웅 삭제(해고) 완료: %s"), *TargetGUID.ToString());

			if (OnInventoryUpdated.IsBound()) OnInventoryUpdated.Broadcast();
			return true;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("❌ [Remove] 해당 GUID를 가진 객체를 찾을 수 없습니다: %s"), *TargetGUID.ToString());
	return false;
}

void UInventoryComponent::Debug_TestInventory()
{
	UE_LOG(LogTemp, Warning, TEXT("=== Inventory Debug Start ==="));

	// -------------------------------------------------------
	// 1. [추가 테스트]
	// -------------------------------------------------------

	// A. 장비 추가 (검 3개 + 10강 검 1개)
	AddItem(FName("Iron_Sword"), 3, 0);
	AddItem(FName("Iron_Sword"), 1, 10);

	// B. 퍼밀리어 추가 (2마리)
	AddFamiliar(FName("test1"));
	AddFamiliar(FName("test1"));

	// C. 영웅 추가 (중복 테스트)
	AddCharacter(FName("Character_Knight"));
	AddCharacter(FName("Character_Knight")); // 중복 방지 로그 확인용


	// -------------------------------------------------------
	// 2. [현재 상태 출력]
	// -------------------------------------------------------
	UE_LOG(LogTemp, Log, TEXT(" "));
	UE_LOG(LogTemp, Log, TEXT("📊 [삭제 전 상태]"));
	for (const auto& Char : OwnedCharacters)
	{
		UE_LOG(LogTemp, Display, TEXT("   [Fam] ID: %s | UID: %s"), *Char.CharacterID.ToString(), *Char.CharacterUID.ToString());
	}
	for (const auto& Item : OwnedItems)
	{
		UE_LOG(LogTemp, Display, TEXT("   [Item] ID: %s | UID: %s | Lv: +%d"),
			*Item.ItemID.ToString(), *Item.ItemUID.ToString(), Item.EnhancementLevel);
	}
	for (const auto& Fam : OwnedFamiliars)
	{
		UE_LOG(LogTemp, Display, TEXT("   [Fam] ID: %s | UID: %s"),
			*Fam.FamiliarID.ToString(), *Fam.FamiliarUID.ToString());
	}


	// -------------------------------------------------------
	// 3. [삭제 테스트] (RemoveObjectByGUID)
	// -------------------------------------------------------
	UE_LOG(LogTemp, Log, TEXT(" "));
	UE_LOG(LogTemp, Warning, TEXT("🗑️ [삭제 테스트 시작]"));

	// A. 아이템 삭제 테스트 (첫 번째 아이템 삭제)
	if (OwnedItems.Num() > 0)
	{
		FGuid TargetItemUID = OwnedItems[0].ItemUID; // 첫 번째 놈 타겟팅
		UE_LOG(LogTemp, Log, TEXT(">> 장비 삭제 시도: %s"), *TargetItemUID.ToString());

		bool bRemoved = RemoveObjectByGUID(TargetItemUID);

		if (bRemoved) {
			UE_LOG(LogTemp, Log, TEXT("   -> 성공!"));
		} 
		else { UE_LOG(LogTemp, Error, TEXT("   -> 실패!")); }
	}

	// B. 퍼밀리어 삭제 테스트 (첫 번째 병사 삭제)
	if (OwnedFamiliars.Num() > 0)
	{
		FGuid TargetFamUID = OwnedFamiliars[0].FamiliarUID; // 첫 번째 놈 타겟팅
		UE_LOG(LogTemp, Log, TEXT(">> 병사 삭제 시도: %s"), *TargetFamUID.ToString());

		bool bRemoved = RemoveObjectByGUID(TargetFamUID);

		if (bRemoved) { UE_LOG(LogTemp, Log, TEXT("   -> 성공!")); }
		else { UE_LOG(LogTemp, Error, TEXT("   -> 실패!")); }
	}


	// -------------------------------------------------------
	// 4. [최종 결과 확인]
	// -------------------------------------------------------
	UE_LOG(LogTemp, Log, TEXT(" "));
	UE_LOG(LogTemp, Log, TEXT("📊 [최종 상태] (검 3개, 병사 1마리 남아야 함)"));
	for (const auto& Char : OwnedCharacters)
	{
		UE_LOG(LogTemp, Display, TEXT("   [Fam] ID: %s | UID: %s"), *Char.CharacterID.ToString(), *Char.CharacterUID.ToString());
	}
	for (const auto& Item : OwnedItems)
	{
		UE_LOG(LogTemp, Display, TEXT("   [Item] ID: %s | UID: %s"), *Item.ItemID.ToString(), *Item.ItemUID.ToString());
	}
	for (const auto& Fam : OwnedFamiliars)
	{
		UE_LOG(LogTemp, Display, TEXT("   [Fam] ID: %s | UID: %s"), *Fam.FamiliarID.ToString(), *Fam.FamiliarUID.ToString());
	}

	UE_LOG(LogTemp, Warning, TEXT("=== Inventory Debug End ==="));
}



int32 UInventoryComponent::GetItemQuantity(FName ItemID) const
{
	if (ItemID.IsNone()) return 0;
	int32 TotalCount = 0;

	for (const auto& Item : OwnedItems)
	{
		if (Item.ItemID == ItemID)
		{
			TotalCount += Item.Quantity; // 장비는 주로 1이지만, 합산 로직 유지
		}
	}
	return TotalCount;
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

FOwnedItemData* UInventoryComponent::GetItemByGUID(FGuid TargetUID)
{
	if (!TargetUID.IsValid()) return nullptr;

	for (auto& Item : OwnedItems)
	{
		if (Item.ItemUID == TargetUID)
		{
			return &Item;
		}
	}
	return nullptr;
}

bool UInventoryComponent::HasCharacter(FName CharacterID) const
{
	for (const auto& Hero : OwnedCharacters)
	{
		if (Hero.CharacterID == CharacterID) return true;
	}
	return false;
}

UParadiseGameInstance* UInventoryComponent::GetParadiseGI() const
{
	return Cast<UParadiseGameInstance>(GetWorld()->GetGameInstance());
}




