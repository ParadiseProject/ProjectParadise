// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SummonSystemComponent.h"
#include "Components/CostManageComponent.h"
#include "Framework/InGame/InGamePlayerState.h"
#include "Characters/AIUnit/BaseUnit.h"
#include "Framework/System/ObjectPoolSubsystem.h"
#include "Data/Structs/UnitStructs.h"
#include "TimerManager.h"

USummonSystemComponent::USummonSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void USummonSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// 게임 시작 시 슬롯 채우기
	RefreshAllSlots();
}

void USummonSystemComponent::RefreshAllSlots()
{
	// 데이터 테이블 유효성 검사
	if (!FamiliarStatsTable || !FamiliarAssetsTable)
	{
		UE_LOG(LogTemp, Error, TEXT("⚠️ [SummonSystem] FamiliarStatsTable 또는 FamiliarAssetsTable이 설정되지 않았습니다."));
		return;
	}

	// 배열 초기화
	CurrentSlots.Empty();

	//5개 슬롯을 랜덤 유닛으로 채움
	for (int32 i = 0; i < MaxSlotCount; i++)
	{
		CurrentSlots.Add(GenerateRandomSlot());
	}

	// UI에 슬롯 갱신 알림(델리게이트 호출)
	if (OnSlotsUpdated.IsBound())
	{
		OnSlotsUpdated.Broadcast(CurrentSlots);
	}
}

bool USummonSystemComponent::TrySummonFamiliar(int32 SlotIndex, FVector SpawnLocation)
{
	//슬롯 유효성 검사
	if (!CurrentSlots.IsValidIndex(SlotIndex)) return false;
	if (CurrentSlots[SlotIndex].bIsEmpty)
	{
		UE_LOG(LogTemp, Warning, TEXT("빈 슬롯입니다. 쿨타임 대기 중.."))
		return false;
	}

	//슬롯 정보 가져오기
	FSummonSlotInfo& SlotInfo = CurrentSlots[SlotIndex];

	//PlayerState 및 CostManger 가져오기
	AInGamePlayerState* PS = GetOwner<AInGamePlayerState>();
	if (!PS) return false;

	UCostManageComponent* CostManager = PS->GetCostManageComponent();

	//비용 비교 및 결제(돈이 부족하면 false처리)
	float PriceToPay = (float)SlotInfo.FamiliarCost; // 혹은 FinalCost
	if (!CostManager || !CostManager->TrySpendCost(PriceToPay))
	{
		UE_LOG(LogTemp, Warning, TEXT("❌ 잔액 부족! (필요: %.0f)"), PriceToPay);
		return false;
	}

	//실제 소환 요청
	//AFamiliarUnit* NewUnit = SpawnFamiliarUnit(SlotInfo.UnitID, SpawnLocation);

	//if (NewUnit)
	//{
	//	//소환 성공 후 슬롯 처리 로직
	//	HandleSuccessfulSummon(SlotIndex);
	//	return true;
	//}

	// 돈은 냈는데 소환 실패? (이런 경우는 거의 없겠지만 안전장치)
	UE_LOG(LogTemp, Error, TEXT("❌ 결제는 성공했으나 유닛 소환 실패!"));
	return false;

}

//AFamiliarUnit* USummonSystemComponent::SpawnFamiliarUnit(FName UnitID, FVector SpawnLocation)
//{
	//오브젝트 풀링으로 소환
	//UObjectPoolSubsystem* PoolSubsystem = GetWorld()->GetSubsystem<UObjectPoolSubsystem>();
	//if (!PoolSubsystem) return false;

	//AActor* SpawnedActor = PoolSubsystem->SpawnFromPool(FamiliarClass, SpawnLocation, FRotator::ZeroRotator);
	//AFamiliarUnit* NewUnit = Cast<AFamiliarUnit>(SpawnedActor);

	//if (NewUnit)
	//{
	//	// 4. 유닛 스탯/에셋 초기화
	//	FFamiliarStats* Stats = FamiliarStatsTable->FindRow<FFamiliarStats>(UnitID, TEXT("Spawn"));
	//	FFamiliarAssets* Assets = FamiliarAssetsTable->FindRow<FFamiliarAssets>(SlotInfo.UnitID, TEXT("Spawn"));

	//	if (Stats && Assets)
	//	{
	//		NewUnit->InitializeFamiliar(*Stats, *Assets);
	//	}
	//	//성공했을떄 유닛을 리턴해줘야 TrySummon 함수가 알 수 있음
	//	return NewUnit;
	//}
	//return nullptr;
//}

void USummonSystemComponent::HandleSuccessfulSummon(int32 SlotIndex)
{
	if (!CurrentSlots.IsValidIndex(SlotIndex)) return;

	FSummonSlotInfo& SlotInfo = CurrentSlots[SlotIndex];

	//슬롯 비우기
	SlotInfo.bIsEmpty = true;
	SlotInfo.UnitID = FName("None"); // 혹은 비어있음 표시
	//SlotInfo.Icon = nullptr;	//아직 없음

	// UI 갱신 (비어있는 상태 보여주기)
	OnSlotsUpdated.Broadcast(CurrentSlots);

	//  쿨타임 타이머 시작 (RefillTimer)
	if (GetWorld())
	{
		FTimerDelegate TimerDel;
		// 어떤 슬롯(SlotIndex)을 채울지 파라미터로 전달
		TimerDel.BindUFunction(this, FName("RefillSpecificSlot"), SlotIndex);

		// 기존 타이머가 있다면 초기화 후 재설정
		//GetWorld()->GetTimerManager().SetTimer(RefillTimers[SlotIndex], TimerDel, SlotRefillCooldown, false);
	}

	//UE_LOG(LogTemp, Log, TEXT("✅ 소환 성공! 슬롯[%d] 쿨타임 시작 (%.1f초)"), SlotIndex, SlotRefillCooldown);

}

void USummonSystemComponent::RefillSpecificSlot(int32 SlotIndex)
{
	if (!CurrentSlots.IsValidIndex(SlotIndex)) return;

	// 1. 새로운 랜덤 유닛 생성
	CurrentSlots[SlotIndex] = GenerateRandomSlot();

	// 2. UI 갱신 알림 (새로 채워진 모습 보여주기)
	OnSlotsUpdated.Broadcast(CurrentSlots);

	UE_LOG(LogTemp, Log, TEXT("🔄 슬롯[%d] 리필 완료!"), SlotIndex);
}

//슬롯을 랜덤으로 for문으로 5번 돌림
FSummonSlotInfo USummonSystemComponent::GenerateRandomSlot()
{
	FSummonSlotInfo NewSlot;

	//데이터테이블 체크
	if (!FamiliarStatsTable) return NewSlot;

	TArray<FName> RowNames = FamiliarStatsTable->GetRowNames();
	if (RowNames.Num() == 0) return NewSlot;

	//랜덤 유닛 ID 뽑기
	int32 RandomIndex = FMath::RandRange(0, RowNames.Num() - 1);
	FName SelectedID = RowNames[RandomIndex];

	FFamiliarStats* Stats = FamiliarStatsTable->FindRow<FFamiliarStats>(SelectedID, TEXT("RandomGen"));

	if (Stats)
	{
		NewSlot.UnitID = SelectedID;
		NewSlot.FamiliarCost = Stats->SummonCost;
		// NewSlot.Icon = Stats->Icon
		NewSlot.bIsEmpty = false;	//꽉 찼음 
	}

	return NewSlot;
}