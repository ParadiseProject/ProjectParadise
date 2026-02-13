// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Panel/Ingame/SummonControlPanel.h"

#include "UI/Widgets/Ingame/SummonSlotWidget.h"
#include "UI/Widgets/Ingame/SummonCostWidget.h"
#include "GameFramework/PlayerController.h"
#include "Framework/InGame/InGamePlayerState.h"

#include "Components/CostManageComponent.h"
#include "Components/FamiliarSummonComponent.h"

#pragma region 생명주기
void USummonControlPanel::NativeConstruct()
{
	Super::NativeConstruct();

	// 배열에 캐싱하여 인덱스로 접근 가능하게 최적화
	SummonSlots.Empty();
	if (SummonSlot_0) SummonSlots.Add(SummonSlot_0);
	if (SummonSlot_1) SummonSlots.Add(SummonSlot_1);
	if (SummonSlot_2) SummonSlots.Add(SummonSlot_2);
	if (SummonSlot_3) SummonSlots.Add(SummonSlot_3);
	if (SummonSlot_4) SummonSlots.Add(SummonSlot_4); 

	// 2. 슬롯 초기화 및 클릭 이벤트 바인딩 (핵심)
	for (int32 i = 0; i < SummonSlots.Num(); ++i)
	{
		if (SummonSlots[i])
		{
			// 인덱스 부여
			SummonSlots[i]->InitSlot(i);

			// 기존 바인딩 제거 (안전장치)
			if (SummonSlots[i]->OnSlotClicked.IsAlreadyBound(this, &USummonControlPanel::HandleSlotClickRequest))
			{
				SummonSlots[i]->OnSlotClicked.RemoveDynamic(this, &USummonControlPanel::HandleSlotClickRequest);
			}
			// 클릭 이벤트 연결
			SummonSlots[i]->OnSlotClicked.AddDynamic(this, &USummonControlPanel::HandleSlotClickRequest);
		}
	}

	InitComponents();
}
void USummonControlPanel::NativeDestruct()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_InitCost);
	}
	if (CachedCostComponent.IsValid())
	{
		CachedCostComponent->OnCostChanged.RemoveAll(this);
	}
	if (CachedSummonComponent.IsValid())
	{
		CachedSummonComponent->OnSummonSlotsUpdated.RemoveAll(this);
	}
	Super::NativeDestruct();
}
#pragma endregion 생명주기

#pragma region 시스템 초기화
void USummonControlPanel::InitComponents()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_InitCost, this, &USummonControlPanel::InitComponents, 0.1f, false);
		return;
	}

	AInGamePlayerState* PS = PC->GetPlayerState<AInGamePlayerState>();
	if (!PS)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_InitCost, this, &USummonControlPanel::InitComponents, 0.1f, false);
		return;
	}

	bool bAllComponentsReady = true;

	// 1. CostManageComponent 연결
	UCostManageComponent* CostComp = PS->GetCostManageComponent();
	if (CostComp)
	{
		CachedCostComponent = CostComp;
		if (!CostComp->OnCostChanged.IsAlreadyBound(this, &USummonControlPanel::HandleCostUpdate))
		{
			CostComp->OnCostChanged.AddDynamic(this, &USummonControlPanel::HandleCostUpdate);
		}
	}
	else
	{
		bAllComponentsReady = false;
	}

	// 2. FamiliarSummonComponent 연결 (소환 로직)
	UFamiliarSummonComponent* SummonComp = PS->GetFamiliarSummonComponent();
	if (SummonComp)
	{
		CachedSummonComponent = SummonComp;
		if (!SummonComp->OnSummonSlotsUpdated.IsAlreadyBound(this, &USummonControlPanel::HandleSummonSlotsUpdate))
		{
			SummonComp->OnSummonSlotsUpdated.AddDynamic(this, &USummonControlPanel::HandleSummonSlotsUpdate);
		}

		// 초기 슬롯 데이터 요청 (필요 시)
		SummonComp->RefreshAllSlots();
	}
	else
	{
		bAllComponentsReady = false;
	}

	// 하나라도 없으면 재시도
	if (bAllComponentsReady)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_InitCost);
		UE_LOG(LogTemp, Log, TEXT("[SummonPanel] 모든 컴포넌트 연결 완료"));
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_InitCost, this, &USummonControlPanel::InitComponents, 0.1f, false);
	}
}
void USummonControlPanel::HandleCostUpdate(float CurrentCost, float MaxCost)
{
	if (CostWidget)
	{
		CostWidget->UpdateCost(CurrentCost, MaxCost);
	}
}
void USummonControlPanel::HandleSummonSlotsUpdate(const TArray<FSummonSlotInfo>& Slots)
{
	// 1. 모든 슬롯 데이터 갱신 (Shift 효과)
	// 배열 전체를 덮어쓰므로, 3번이 2번으로 가면 2번 슬롯 이미지가 3번 유닛으로 바뀜 -> 이동한 것처럼 보임
	for (int32 i = 0; i < SummonSlots.Num(); ++i)
	{
		if (SummonSlots.IsValidIndex(i) && SummonSlots[i] && Slots.IsValidIndex(i))
		{
			// 동기 로드 (아이콘은 가벼운 에셋이라 가정)
			UTexture2D* LoadedIcon = Slots[i].FamiliarIcon.LoadSynchronous();
			int32 Cost = Slots[i].FamiliarCost;

			SummonSlots[i]->UpdateSlotInfo(LoadedIcon, Cost);
		}
	}

	// 2. ★ [핵심] 마지막 슬롯 애니메이션 (Append 효과)
	// Shift & Append 로직상 마지막 슬롯(Index 4)은 항상 '새로 들어온 유닛'입니다.
	int32 LastIndex = SummonSlots.Num() - 1;
	if (SummonSlots.IsValidIndex(LastIndex) && SummonSlots[LastIndex])
	{
		// 마지막 슬롯만 튀어나오게 하여 "추가됨"을 강조
		SummonSlots[LastIndex]->PlayIntroAnimation();
	}
}
#pragma endregion 시스템 초기화

#pragma region 입력 처리
void USummonControlPanel::HandleSlotClickRequest(int32 SlotIndex)
{
	if (CachedSummonComponent.IsValid())
	{
		// MVC: View(Panel) -> Controller(Component) 요청
		// 성공 여부에 따른 UI 갱신은 Delegate(HandleSummonSlotsUpdate)가 처리
		bool bSuccess = CachedSummonComponent->RequestPurchase(SlotIndex);

		if (!bSuccess)
		{
			UE_LOG(LogTemp, Warning, TEXT("[SummonPanel] 구매 실패: %d"), SlotIndex);
			// 여기에 실패 피드백 (예: 흔들림 애니메이션) 추가 가능
		}
	}
}
#pragma endregion 입력 처리

#pragma region 외부 인터페이스 구현
void USummonControlPanel::SetSummonSlotData(int32 SlotIndex, UTexture2D* Icon, int32 InCost)
{
	if (SummonSlots.IsValidIndex(SlotIndex) && SummonSlots[SlotIndex])
	{
		SummonSlots[SlotIndex]->UpdateSlotInfo(Icon, InCost);
	}
}

//void USummonControlPanel::UpdateSummonCooldown(int32 SlotIndex, float CurrentTime, float MaxTime)
//{
//	//if (SummonSlots.IsValidIndex(SlotIndex) && SummonSlots[SlotIndex])
//	//{
//	//	//SummonSlots[SlotIndex]->RefreshCooldown(CurrentTime, MaxTime);
//	//}
//}
void USummonControlPanel::UpdateCostDisplay(float CurrentCost, float MaxCost)
{
	if (CostWidget)
	{
		CostWidget->UpdateCost(CurrentCost, MaxCost);
	}
}
#pragma endregion 외부 인터페이스 구현
