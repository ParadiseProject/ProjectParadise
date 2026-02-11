// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/FamiliarSummonComponent.h"
#include "Components/CostManageComponent.h"
#include "Framework/InGame/InGamePlayerState.h"
#include "Characters/AIUnit/BaseUnit.h"
#include "Framework/System/ObjectPoolSubsystem.h"
#include "Data/Structs/UnitStructs.h"
#include "TimerManager.h"

UFamiliarSummonComponent::UFamiliarSummonComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFamiliarSummonComponent::BeginPlay()
{
	Super::BeginPlay();

	// 게임 시작 시 슬롯 채우기
	RefreshAllSlots();
}

void UFamiliarSummonComponent::RefreshAllSlots()
{
	// 데이터 테이블 유효성 검사
	if (!FamiliarStatsTable || !FamiliarAssetsTable)
	{
		UE_LOG(LogTemp, Error, TEXT("⚠️ [FamiliarSummon] FamiliarStatsTable 또는 FamiliarAssetsTable이 설정되지 않았습니다."));
		return;
	}

	// 배열 초기화
	CurrentSlots.Empty();

	UE_LOG(LogTemp, Warning, TEXT("========== 🎰 [상점 리스트 갱신] 🎰 =========="));
	
	//5개 슬롯을 랜덤 유닛으로 채움
	for (int32 i = 0; i < MaxSlotCount; i++)
	{
		//슬롯하나 생성 
		FSummonSlotInfo NewSlot = GenerateRandomSlot();
		//배열에 추가
		CurrentSlots.Add(NewSlot);

		// [디버깅용] 가격 슬롯 1-5까지 출력
		UE_LOG(LogTemp, Log, TEXT("[%d번 키] 유닛: %s | 가격: %d"),
			i + 1, *NewSlot.FamiliarID.ToString(), NewSlot.FamiliarCost);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("=============================================="));

	// UI에 슬롯 갱신 알림(델리게이트 호출)
	if (OnSummonSlotsUpdated.IsBound())	OnSummonSlotsUpdated.Broadcast(CurrentSlots);
}

bool UFamiliarSummonComponent::RequestPurchase(int32 SlotIndex)
{
	//슬롯 유효성 검사
	if (!CurrentSlots.IsValidIndex(SlotIndex)) return false;
	if (CurrentSlots[SlotIndex].bIsSoldOut)
	{
		UE_LOG(LogTemp, Warning, TEXT("빈 슬롯입니다. 쿨타임 대기 중.."))
		return false;
	}

	//PlayerState 및 CostManger 가져오기
	AInGamePlayerState* PS = GetOwner<AInGamePlayerState>();
	if (!PS) return false;

	UCostManageComponent* CostManager = PS->GetCostManageComponent();
	//슬롯 정보 가져오기
	FSummonSlotInfo& SlotInfo = CurrentSlots[SlotIndex];

	//비용 비교 및 결제(돈이 부족하면 false처리)
	float PriceToPay = (float)SlotInfo.FamiliarCost; // 혹은 FinalCost
	if (!CostManager || !CostManager->TrySpendCost(PriceToPay))
	{
		UE_LOG(LogTemp, Warning, TEXT("❌ 잔액 부족! (필요: %.0f)"), PriceToPay);
		return false;
	}

	//FVector SpawnLocation = FVector::ZeroVector;

	//실제 소환 요청
	//AFamiliarUnit* NewUnit = SpawnFamiliarUnit(SlotInfo.UnitID, SpawnLocation);

	//if (NewUnit)
	//{
		UE_LOG(LogTemp, Log, TEXT("📍 구매 승인 (유닛 스폰 생략)"));
		//소환 성공 후 슬롯 처리 로직
		ConsumeSpecificSlot(SlotIndex);
		return true;
	//}

	// 돈은 냈는데 소환 실패? (이런 경우는 거의 없겠지만 안전장치)
	//UE_LOG(LogTemp, Error, TEXT("❌ 결제는 성공했으나 유닛 소환 실패!"));
	//return false;

}


//구매한 슬롯 비우고 쿨타임시작
void UFamiliarSummonComponent::ConsumeSpecificSlot(int32 SlotIndex)
{
	if (!CurrentSlots.IsValidIndex(SlotIndex)) return;

	FSummonSlotInfo& SlotInfo = CurrentSlots[SlotIndex];

	//슬롯 비우기
	SlotInfo.bIsSoldOut = true;
	SlotInfo.FamiliarID = FName("None"); // 혹은 비어있음 표시
	//SlotInfo.Icon = nullptr;	//아직 없음

	// UI 갱신 (비어있는 상태 보여주기)
	OnSummonSlotsUpdated.Broadcast(CurrentSlots);

	//  쿨타임 타이머 시작 (RefillTimer)
	if (GetWorld())
	{
		FTimerDelegate TimerDel;
		// 어떤 슬롯(SlotIndex)을 채울지 파라미터로 전달
		TimerDel.BindUFunction(this, FName("RefillSpecificSlot"), SlotIndex);

		// 기존 타이머가 있다면 초기화 후 재설정
		GetWorld()->GetTimerManager().SetTimer(RefillTimers[SlotIndex], TimerDel, RefillCooldownTime, false);
	}

	UE_LOG(LogTemp, Log, TEXT("✅ 소환 성공! 슬롯[%d] 쿨타임 시작 (%.1f초)"), SlotIndex, RefillCooldownTime);

}

//슬롯 채우기 
void UFamiliarSummonComponent::RefillSpecificSlot(int32 SlotIndex)
{
	if (!CurrentSlots.IsValidIndex(SlotIndex)) return;

	// 1. 새로운 랜덤 유닛 생성
	CurrentSlots[SlotIndex] = GenerateRandomSlot();

	// 2. UI 갱신 알림 (새로 채워진 모습 보여주기)
	OnSummonSlotsUpdated.Broadcast(CurrentSlots);

	UE_LOG(LogTemp, Log, TEXT("🔄 슬롯[%d] 리필 완료!"), SlotIndex);

	// 방금 채워진 슬롯의 정보를 가져옴
	const FSummonSlotInfo& NewSlot = CurrentSlots[SlotIndex];

	// 로그 출력 (예: 🔄 [리필 완료] 슬롯[1] -> 유닛: Wolf_01 | 가격: 50)
	UE_LOG(LogTemp, Warning, TEXT("🔄 [리필 완료] 슬롯[%d] -> 유닛: %s | 가격: %d"),
		SlotIndex + 1, *NewSlot.FamiliarID.ToString(), NewSlot.FamiliarCost);
}

//슬롯을 랜덤으로 for문으로 5번 돌림
FSummonSlotInfo UFamiliarSummonComponent::GenerateRandomSlot()
{
	FSummonSlotInfo NewSlot;

	//데이터테이블 체크
	if (!FamiliarStatsTable) return NewSlot;

	TArray<FName> RowNames = FamiliarStatsTable->GetRowNames();
	if (RowNames.Num() == 0) return NewSlot;

	//랜덤 유닛 ID 뽑기
	int32 RandomIndex = FMath::RandRange(0, RowNames.Num() - 1);
	FName SelectedID = RowNames[RandomIndex];

	//데이터 테이블에서 정보 가져오기
	FFamiliarStats* Stats = FamiliarStatsTable->FindRow<FFamiliarStats>(SelectedID, TEXT("RandomGen"));

	if (Stats)
	{
		NewSlot.FamiliarID = SelectedID;
		NewSlot.FamiliarCost = Stats->SummonCost;
		// NewSlot.Icon = Stats->Icon
		NewSlot.bIsSoldOut = false;	//꽉 찼음 
	}
	return NewSlot;
}

//AFamiliarUnit* UFamiliarSummonComponent::SpawnFamiliarUnit(FName UnitID, FVector SpawnLocation)
//{
	//오브젝트 풀링으로 소환

//}