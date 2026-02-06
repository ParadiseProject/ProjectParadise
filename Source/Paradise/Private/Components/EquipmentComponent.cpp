// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/EquipmentComponent.h"
#include "Components/InventoryComponent.h"
#include "Data/Structs/ItemStructs.h"
#include "Data/Structs/InventoryStruct.h"
#include "Framework/Core/ParadiseGameInstance.h"
#include "Characters/Base/PlayerBase.h"
#include "Engine/StaticMeshActor.h" //제거예정

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


void UEquipmentComponent::SetLinkedInventory(UInventoryComponent* InInventory)
{

}

// Called when the game starts
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}

void UEquipmentComponent::EquipItem(FName ItemID)
{
    UParadiseGameInstance* GI = GetWorld()->GetGameInstance<UParadiseGameInstance>();
    if (!GI)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ [Equip] GameInstance is NULL"));
        return;
    }

    bool bEquipped = false; // 장착 성공 여부 체크용 플래그


    // 1. 무기 테이블(WeaponStats) 검색
    if (FWeaponStats* WeaponRow = GI->GetDataTableRow<FWeaponStats>(GI->WeaponStatsDataTable, ItemID))
    {
        EEquipmentSlot TargetSlot = EEquipmentSlot::Weapon;

        // 장착 (덮어쓰기)
        FOwnedItemData NewEquip;
        NewEquip.ItemID = ItemID;
        NewEquip.Quantity = 1;
        EquippedItems.Add(TargetSlot, NewEquip.ItemID);

        UE_LOG(LogTemp, Log, TEXT("⚔️ [Equip] 무기 장착 성공: %s"), *ItemID.ToString());
        bEquipped = true;
    }

    // 2. 방어구 테이블(ArmorStats) 검색
    else if (FArmorStats* ArmorRow = GI->GetDataTableRow<FArmorStats>(GI->ArmorStatsDataTable, ItemID))
    {
        // [임시] 슬롯 결정 로직 (태그 확인 또는 테이블에 컬럼 추가 권장)
        EEquipmentSlot TargetSlot = EEquipmentSlot::Chest;

        // 예: 태그로 슬롯 찾기
        /*
        if (ArmorRow->ArmorTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Type.Armor.Helmet"))) TargetSlot = EEquipmentSlot::Helmet;
        else if (ArmorRow->ArmorTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Type.Armor.Boots"))) TargetSlot = EEquipmentSlot::Boots;
        */

        EquippedItems.Add(TargetSlot, ItemID);

        UE_LOG(LogTemp, Log, TEXT("🛡️ [Equip] 방어구 장착 성공: %s (Slot: %d)"), *ItemID.ToString(), (int32)TargetSlot);
        bEquipped = true;
    }
    else
    {
        // 3. 실패 처리
        UE_LOG(LogTemp, Warning, TEXT("❌ [Equip] 장착 실패: %s (테이블에 없음)"), *ItemID.ToString());
        return;
    }

    // ✅ [로그] 현재 장착된 아이템 전체 리스트 출력
    if (bEquipped)
    {
        UE_LOG(LogTemp, Warning, TEXT("=== 🎒 현재 장착 리스트 (Total: %d) ==="), EquippedItems.Num());

        // TMap 순회하며 출력
        const UEnum* SlotEnum = StaticEnum<EEquipmentSlot>(); // Enum 이름 문자열 변환용
        for (const auto& Pair : EquippedItems)
        {
            FString SlotName = SlotEnum ? SlotEnum->GetNameStringByValue((int64)Pair.Key) : FString::FromInt((int32)Pair.Key);
            UE_LOG(LogTemp, Log, TEXT("   🔹 [%s] : %s"), *SlotName, *Pair.Value.ToString());
        }
        UE_LOG(LogTemp, Warning, TEXT("======================================"));

        // UI 업데이트 알림
        if (OnEquipmentUpdated.IsBound()) OnEquipmentUpdated.Broadcast();
    }
}

void UEquipmentComponent::UnEquipItem(EEquipmentSlot Slot)
{
}

FName UEquipmentComponent::GetEquippedItemID(EEquipmentSlot Slot) const
{
	return EquippedItems.FindRef(Slot);
}

void UEquipmentComponent::SetEquippedItems(const TMap<EEquipmentSlot, FName>& InItems)
{
}


void UEquipmentComponent::UpdateVisuals(APlayerBase* TargetCharacter)
{
    if (!TargetCharacter) return;

    // [테스트] 무조건 큐브 무기 장착 시도
    // 실제로는 GetEquippedItemID()로 ID를 가져와야 함
    FName TestWeaponID = FName("TestCubeSword");
    AttachWeaponActor(TargetCharacter, TestWeaponID);
}


void UEquipmentComponent::AttachWeaponActor(APlayerBase* Char, FName ItemID)
{
    if (!Char || !GetWorld()) return;

    // 1. [임시] 엔진 기본 큐브 메쉬 로드 (테스트용)
    UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));

    if (!CubeMesh)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ 큐브 메쉬를 찾을 수 없습니다!"));
        return;
    }

    // 2. 스태틱 메쉬 액터 스폰 (임시 무기 액터)
    FActorSpawnParameters Params;
    Params.Owner = Char;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AStaticMeshActor* NewWeapon = GetWorld()->SpawnActor<AStaticMeshActor>(
        AStaticMeshActor::StaticClass(), 
        FVector::ZeroVector, 
        FRotator::ZeroRotator, 
        Params);

    if (NewWeapon)
    {
        // 3. 큐브 모양 적용 및 설정
        NewWeapon->GetStaticMeshComponent()->SetStaticMesh(CubeMesh);
        NewWeapon->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable); // 움직여야 하므로 Movable 필수
        NewWeapon->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 캐릭터랑 충돌 안 나게

        // 크기 조절 (큐브가 너무 크니까 좀 줄임)
        NewWeapon->SetActorScale3D(FVector(0.2f, 0.2f, 1.0f)); // 칼처럼 길쭉하게

        // 4. 캐릭터에게 부착 요청
        // "Weapon_R" 소켓이 없으면 "hand_r"이라도 쓰도록 임시 처리 (소켓 이름을 확인해주세요!)
        FName SocketToUse = Char->GetMesh()->DoesSocketExist(TEXT("Socket_Weapon_R")) ? TEXT("Socket_Weapon_R") : TEXT("hand_r");

        Char->AttachWeapon(NewWeapon, SocketToUse);
    }
}

void UEquipmentComponent::SetArmorMesh(APlayerBase* Char, EEquipmentSlot Slot, FName ItemID)
{
}




