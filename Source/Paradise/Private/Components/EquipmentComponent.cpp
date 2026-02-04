// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/EquipmentComponent.h"
#include "Components/InventoryComponent.h"
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
}

void UEquipmentComponent::UnEquipItem(EEquipmentSlot Slot)
{
}

FName UEquipmentComponent::GetEquippedItemID(EEquipmentSlot Slot) const
{
	return FName();
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




