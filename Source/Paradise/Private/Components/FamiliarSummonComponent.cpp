// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/FamiliarSummonComponent.h"
#include "Components/CostManageComponent.h"
#include "Framework/InGame/InGamePlayerState.h"
#include "Characters/AIUnit/UnitBase.h"
#include "Framework/Core/ParadiseGameInstance.h"
#include "Objects/FamiliarSpawner.h"
#include "Kismet/GameplayStatics.h"
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

	// 게임 시작 시 월드에 있는 스포너 찾아서 저장함
	AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), AFamiliarSpawner::StaticClass());
	if (FoundActor)
	{
		LinkedSpawner = Cast<AFamiliarSpawner>(FoundActor);
		UE_LOG(LogTemp, Log, TEXT("✅ 스포너를 찾아서 연결했습니다."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ 월드에 배치된 AFamiliarSpawner가 없습니다!"));
	}
}

void UFamiliarSummonComponent::RefreshAllSlots()
{
	// 1. GameInstance 가져오기
	UParadiseGameInstance* GI = Cast<UParadiseGameInstance>(GetWorld()->GetGameInstance());
	if (!GI) return;

	// 2. 인스턴스에 선언된 변수명 그대로 사용
	UDataTable* StatsTable = GI->FamiliarStatsDataTable;
	UDataTable* AssetsTable = GI->FamiliarAssetsDataTable;

	if (!StatsTable || !AssetsTable) return;
	UE_LOG(LogTemp, Warning, TEXT("========== 🎰 [상점 리스트 갱신] 🎰 =========="));
	
	CurrentSlots.Empty();

	//5개 슬롯을 랜덤 유닛으로 채움
	for (int32 i = 0; i < MaxSlotCount; i++)
	{
		//슬롯하나 생성 
		FSummonSlotInfo NewSlot = GenerateRandomSlot(StatsTable, AssetsTable);
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
	//if (CurrentSlots[SlotIndex].bIsSoldOut)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("빈 슬롯입니다. 쿨타임 대기 중.."))
	//		return false;
	//}

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

	if (LinkedSpawner)
	{
		//ID를 넘겨서 실제 소환 수행
		LinkedSpawner->SpawnFamiliarByID(SlotInfo.FamiliarID);

		//소환 성공 후 슬롯 처리 로직
		ConsumeSpecificSlot(SlotIndex);
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ 월드에서 AFamiliarSpawner를 찾을 수 없습니다!"));
		return false;
	}
}


void UFamiliarSummonComponent::RegisterSpawner(AFamiliarSpawner* NewSpanwer)
{
	LinkedSpawner = NewSpanwer;
	UE_LOG(LogTemp, Warning, TEXT("✅ Familiar Spawner Linked 성공"));
}

//구매한 슬롯 제거하고 맨 뒤에 채우는 함수(Queue 방식)
void UFamiliarSummonComponent::ConsumeSpecificSlot(int32 SlotIndex)
{
	if (!CurrentSlots.IsValidIndex(SlotIndex)) return;

	//선택한 슬롯을 배열에서 제거
	//TArray의 RemoveAt 쓰면 요소들이 자동으로 앞으로 한칸씩 당겨짐
	CurrentSlots.RemoveAt(SlotIndex);

	//데이터테이블 가져오기
	UParadiseGameInstance* GI = Cast<UParadiseGameInstance>(GetWorld()->GetGameInstance());
	if (GI || GI->FamiliarStatsDataTable || GI->FamiliarAssetsDataTable)
	{
		//1. 새로운 랜덤 유닛 생성
		FSummonSlotInfo NewSlot = GenerateRandomSlot(GI->FamiliarStatsDataTable, GI->FamiliarAssetsDataTable);

		//2. 배열 맨 뒤에 추가
		CurrentSlots.Add(NewSlot);

		UE_LOG(LogTemp, Log, TEXT("🔥 슬롯[%d] 구매 완료 -> 목록 당겨짐 -> 맨 뒤에 [%s] 추가됨"),
			SlotIndex, *NewSlot.FamiliarID.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameInstance나 DataTable을 찾을 수 없어 새 슬롯을 추가하지 못했습니다."));
	}
	if (OnSummonSlotsUpdated.IsBound())
	{
		OnSummonSlotsUpdated.Broadcast(CurrentSlots);
	}
}

//	FSummonSlotInfo& SlotInfo = CurrentSlots[SlotIndex];
//
//	//슬롯 비우기
//	SlotInfo.bIsSoldOut = true;
//	SlotInfo.FamiliarID = FName("None"); // 혹은 비어있음 표시
//	//SlotInfo.Icon = nullptr;	//아직 없음
//
//	// UI 갱신 (비어있는 상태 보여주기)
//	OnSummonSlotsUpdated.Broadcast(CurrentSlots);
//
//	//  쿨타임 타이머 시작 (RefillTimer)
//	if (GetWorld())
//	{
//		FTimerDelegate TimerDel;
//		// 어떤 슬롯(SlotIndex)을 채울지 파라미터로 전달
//		TimerDel.BindUFunction(this, FName("RefillSpecificSlot"), SlotIndex);
//
//		// 기존 타이머가 있다면 초기화 후 재설정
//		GetWorld()->GetTimerManager().SetTimer(RefillTimers[SlotIndex], TimerDel, RefillCooldownTime, false);
//	}
//
//	UE_LOG(LogTemp, Log, TEXT("✅ 소환 성공! 슬롯[%d] 쿨타임 시작 (%.1f초)"), SlotIndex, RefillCooldownTime);
//
//}

//슬롯 채우기 
void UFamiliarSummonComponent::RefillSpecificSlot(int32 SlotIndex)
{
	//if (!CurrentSlots.IsValidIndex(SlotIndex)) return;

	//UParadiseGameInstance* GI = Cast<UParadiseGameInstance>(GetWorld()->GetGameInstance());
	//if (!GI || !GI->FamiliarStatsDataTable || !GI->FamiliarAssetsDataTable) return;

	//// 1. 새로운 랜덤 유닛 생성
	//CurrentSlots[SlotIndex] = GenerateRandomSlot(GI->FamiliarStatsDataTable, GI->FamiliarAssetsDataTable);

	//// 2. UI 갱신 알림 (새로 채워진 모습 보여주기)
	//OnSummonSlotsUpdated.Broadcast(CurrentSlots);

	//UE_LOG(LogTemp, Log, TEXT("🔄 슬롯[%d] 리필 완료!"), SlotIndex);

	//// 방금 채워진 슬롯의 정보를 가져옴
	//const FSummonSlotInfo& NewSlot = CurrentSlots[SlotIndex];

	//// 로그 출력 (예: 🔄 [리필 완료] 슬롯[1] -> 유닛: Wolf_01 | 가격: 50)
	//UE_LOG(LogTemp, Warning, TEXT("🔄 [리필 완료] 슬롯[%d] -> 유닛: %s | 가격: %d"),
	//	SlotIndex + 1, *NewSlot.FamiliarID.ToString(), NewSlot.FamiliarCost);
}

//슬롯을 랜덤으로 for문으로 5번 돌림
FSummonSlotInfo UFamiliarSummonComponent::GenerateRandomSlot(UDataTable* StatsTable, UDataTable* AssetsTable)
{
	FSummonSlotInfo NewSlot;
	TArray<FName> RowNames = StatsTable->GetRowNames();

	int32 RandomIndex = FMath::RandRange(0, RowNames.Num() - 1);
	FName SelectedID = RowNames[RandomIndex];

	FFamiliarStats* Stats = StatsTable->FindRow<FFamiliarStats>(SelectedID, TEXT(""));
	FFamiliarAssets* Assets = AssetsTable->FindRow<FFamiliarAssets>(SelectedID, TEXT(""));

	if (Stats && Assets)
	{
		NewSlot.FamiliarID = SelectedID;
		NewSlot.FamiliarCost = Stats->SummonCost;
		//NewSlot.UnitIcon = Stats->UnitIcon; // 필요시
		NewSlot.bIsSoldOut = false;
	}
	return NewSlot;
}
