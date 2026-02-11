// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/EquipmentComponent.h"
#include "Components/InventoryComponent.h"
#include "Framework/Core/ParadiseGameInstance.h"
#include "Characters/Base/PlayerBase.h"
#include "Characters/Player/PlayerData.h"
#include "Animation/SkeletalMeshActor.h"
#include "Engine/StaticMeshActor.h"

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
    if (InInventory)
    {
        LinkedInventory = InInventory;
        UE_LOG(LogTemp, Log, TEXT("🔗 [Equipment] 인벤토리 연결 성공!"));
    }
}

// Called when the game starts
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}

FName UEquipmentComponent::GetEquippedItemID(EEquipmentSlot Slot) const
{
    //해당 슬롯에 GUID가 없으면 None
    if (!EquippedItems.Contains(Slot)) return NAME_None;

    FGuid TargetUID = EquippedItems[Slot];

    //인벤토리가 없으면 조회를 못하므로 None
    if (!LinkedInventory) return NAME_None;

    //인벤토리에게 물어봐서 데이터 가져오기
    if (FOwnedItemData* ItemData = LinkedInventory->GetItemByGUID(TargetUID))
    {
        //인벤토리에서 찾아서 FName 반환
        return ItemData->ItemID;
    }

    return NAME_None;
}

bool UEquipmentComponent::GetEquippedItemData(EEquipmentSlot Slot, FOwnedItemData& OutData) const
{
    if (!LinkedInventory) return false;

    if (const FGuid* FoundGUID = EquippedItems.Find(Slot))
    {
        if (FOwnedItemData* RealData = LinkedInventory->GetItemByGUID(*FoundGUID))
        {
            OutData = *RealData;
            return true;
        }
    }
    return false;
}

void UEquipmentComponent::UpdateVisuals(APlayerBase* TargetCharacter)
{
	// 타겟이 없으면 컴포넌트 소유자를 사용
	APlayerBase* Char = TargetCharacter ? TargetCharacter : Cast<APlayerBase>(GetOwner());
	if (!Char || !LinkedInventory) return;

	UE_LOG(LogTemp, Log, TEXT("🎨 [Visual] 캐릭터 외형 업데이트 시작..."));

	//무기 처리 (Weapon Slot)
	FOwnedItemData WeaponData;
	if (GetEquippedItemData(EEquipmentSlot::Weapon, WeaponData))
	{
		// 무기 데이터가 있으면 액터 생성 및 부착
		AttachWeaponActor(Char, WeaponData.ItemID);
	}
	else
	{
		// 무기가 없으면 기존 액터 파괴
		if (SpawnedWeaponActor)
		{
			SpawnedWeaponActor->Destroy();
			SpawnedWeaponActor = nullptr;
		}
	}

	//방어구 처리 (Armor Slots)
	// (필요한 모든 방어구 슬롯을 순회합니다)
	const TArray<EEquipmentSlot> ArmorSlots = {
		EEquipmentSlot::Helmet,
		EEquipmentSlot::Chest,
		EEquipmentSlot::Gloves,
		EEquipmentSlot::Boots
	};

	for (EEquipmentSlot Slot : ArmorSlots)
	{
		FOwnedItemData ArmorData;
		// 해당 슬롯에 장착된 아이템이 있는지 확인
		if (GetEquippedItemData(Slot, ArmorData))
		{
			SetArmorMesh(Char, Slot, ArmorData.ItemID);
		}
		else
		{
			// 장착 해제 상태라면 빈 이름(NAME_None)을 전달하여 메쉬를 비움
			SetArmorMesh(Char, Slot, NAME_None);
		}
	}
}

void UEquipmentComponent::AttachWeaponActor(APlayerBase* Char, FName ItemID)
{
	UParadiseGameInstance* GI = Cast<UParadiseGameInstance>(GetWorld()->GetGameInstance());
	if (!GI || !Char) return;

	//데이터 테이블 조회
	FWeaponAssets* WeaponAssets = GI->GetDataTableRow<FWeaponAssets>(GI->WeaponAssetsDataTable, ItemID);
	if (!WeaponAssets) return;

	//기존 무기 제거 (교체 시)
	if (SpawnedWeaponActor)
	{
		SpawnedWeaponActor->Destroy();
		SpawnedWeaponActor = nullptr;
	}

	//메쉬 로드 (SoftReference -> Hard Load)
	USkeletalMesh* LoadedMesh = WeaponAssets->ItemMesh.LoadSynchronous();
	if (!LoadedMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("⚠️ [Visual] 무기 메쉬 로드 실패: %s"), *ItemID.ToString());
		return;
	}

	//무기 액터 스폰 (SkeletalMeshActor 사용)
	FActorSpawnParameters Params;
	Params.Owner = Char;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ASkeletalMeshActor* NewWeapon = GetWorld()->SpawnActor<ASkeletalMeshActor>(
		ASkeletalMeshActor::StaticClass(),
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		Params);

	if (NewWeapon)
	{
		USkeletalMeshComponent* WeaponComp = NewWeapon->GetSkeletalMeshComponent();
		if (WeaponComp)
		{
			WeaponComp->SetSkeletalMesh(LoadedMesh);
			WeaponComp->SetMobility(EComponentMobility::Movable);
			WeaponComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 캐릭터 충돌 방지
		}

		FName SocketName = WeaponAssets->AttachmentSocket;

		UE_LOG(LogTemp, Warning, TEXT("🔍 [Debug] 아이템: %s | 테이블 소켓값: '%s'"),
			*ItemID.ToString(), *SocketName.ToString());

		//소켓 값이 비어있는 경우 (None)
		if (SocketName.IsNone())
		{
			UE_LOG(LogTemp, Warning, TEXT("⚠️ [Debug] 소켓 이름이 None입니다. 기본값 'hand_r'을 사용합니다."));
			SocketName = TEXT("hand_r");
		}
		//소켓 값이 있는 경우
		else
		{
			//캐릭터 메쉬에 해당 소켓이 진짜 있는지 확인
			if (Char->GetMesh()->DoesSocketExist(SocketName))
			{
				UE_LOG(LogTemp, Log, TEXT("✅ [Debug] 소켓 '%s' 존재 확인됨. 부착 시도."), *SocketName.ToString());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("❌ [Debug] 소켓 '%s'이(가) 캐릭터 메쉬에 없습니다! (오타 확인 필요)"), *SocketName.ToString());
			}
		}

		// 최종 부착
		NewWeapon->AttachToComponent(Char->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);

		SpawnedWeaponActor = NewWeapon;

		UE_LOG(LogTemp, Log, TEXT("⚔️ [Visual] 무기 장착 완료: %s"), *ItemID.ToString());
	}
}

void UEquipmentComponent::SetArmorMesh(APlayerBase* Char, EEquipmentSlot Slot, FName ItemID)
{
	if (!Char) return;

	//해당 슬롯의 메쉬 컴포넌트 가져오기
	USkeletalMeshComponent* TargetMeshComp = Char->GetArmorComponent(Slot);
	if (!TargetMeshComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("⚠️ [Visual] 캐릭터에 해당 슬롯(%d) 컴포넌트가 없습니다."), (int32)Slot);
		return;
	}

	//장착 해제 처리 (None)
	if (ItemID.IsNone())
	{
		//초기화
		TargetMeshComp->SetSkeletalMesh(nullptr);
		TargetMeshComp->SetLeaderPoseComponent(nullptr);
		TargetMeshComp->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		return;
	}

	UParadiseGameInstance* GI = Cast<UParadiseGameInstance>(GetWorld()->GetGameInstance());
	if (!GI) return;

	//데이터 테이블 조회
	FArmorAssets* ArmorAssets = GI->GetDataTableRow<FArmorAssets>(GI->ArmorAssetsDataTable, ItemID);
	if (!ArmorAssets) return;

	//메쉬 로드
	USkeletalMesh* LoadedMesh = ArmorAssets->ItemMesh.LoadSynchronous();
	if (!LoadedMesh) return;

	//메쉬 적용
	TargetMeshComp->SetSkeletalMesh(LoadedMesh);

	//소켓부착
	FName SocketName = ArmorAssets->AttachmentSocket;

	//소켓 이름이 지정된 경우
	if (!SocketName.IsNone())
	{
		//리더 포즈 해제 (중복 적용 방지)
		TargetMeshComp->SetLeaderPoseComponent(nullptr);

		// 소켓 존재 여부 확인
		if (Char->GetMesh()->DoesSocketExist(SocketName))
		{
			// 3) 소켓에 부착
			TargetMeshComp->AttachToComponent(Char->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
			UE_LOG(LogTemp, Log, TEXT("🛡️ [Visual] 방어구 소켓 부착 완료: %s -> %s"), *ItemID.ToString(), *SocketName.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("❌ [Visual] 소켓을 찾을 수 없음: %s (Item: %s)"), *SocketName.ToString(), *ItemID.ToString());
		}
	}
	//소켓 이름이 없는 경우 (일반 방어구 - 리더 포즈)
	else
	{
		//부착 해제 (혹시 붙어있었다면)
		TargetMeshComp->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);

		//부모 메쉬에 다시 붙임
		TargetMeshComp->SetupAttachment(Char->GetMesh());

		//리더 포즈 설정 (애니메이션 동기화)
		TargetMeshComp->SetLeaderPoseComponent(Char->GetMesh());

		UE_LOG(LogTemp, Log, TEXT("🛡️ [Visual] 방어구 리더 포즈 적용: %s (Slot: %d)"), *ItemID.ToString(), (int32)Slot);
	}
}




