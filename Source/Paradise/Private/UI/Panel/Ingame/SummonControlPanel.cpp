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

			// 데이터가 있으면 가져오고, 없어도 그냥 0/null로 밀어넣음
			if (Slots.IsValidIndex(i))
			{
				LoadedIcon = Slots[i].FamiliarIcon.LoadSynchronous();
				Cost = Slots[i].FamiliarCost;
			}

			SummonSlots[i]->UpdateSlotInfo(LoadedIcon, Cost);
		}
	}
	// 2. [애니메이션 재생 분기 처리]
	if (LastClickedSlotIndex >= 0)
	{
		// A. 내가 누른 슬롯부터 마지막 직전 슬롯까지는 "당겨오기(Shift)" 애니메이션
		// 예: 2번을 눌렀으면 2, 3번 위젯이 당겨지는 애니메이션을 재생함
		for (int32 i = LastClickedSlotIndex; i < SummonSlots.Num() - 1; ++i)
		{
			if (SummonSlots.IsValidIndex(i) && SummonSlots[i])
			{
				SummonSlots[i]->PlayShiftAnimation();
			}
		}

		// B. 맨 마지막 슬롯은 "새로 등장(Intro)" 애니메이션
		int32 LastIndex = SummonSlots.Num() - 1;
		if (SummonSlots.IsValidIndex(LastIndex) && SummonSlots[LastIndex])
		{
			SummonSlots[LastIndex]->PlayIntroAnimation();
		}

		// 애니메이션 처리 끝났으므로 인덱스 초기화
		LastClickedSlotIndex = -1;
	}
}
#pragma endregion 시스템 초기화

#pragma region 입력 처리
void USummonControlPanel::HandleSlotClickRequest(int32 SlotIndex)
{
	if (CachedSummonComponent.IsValid())
	{
		// ★ [추가] 클릭한 인덱스를 기억해둡니다 (애니메이션 재생용)
		LastClickedSlotIndex = SlotIndex;

		bool bSuccess = CachedSummonComponent->RequestPurchase(SlotIndex);

		if (!bSuccess)
		{
			// 실패하면 초기화
			LastClickedSlotIndex = -1;
			UE_LOG(LogTemp, Warning, TEXT("[SummonPanel] 구매 실패: %d"), SlotIndex);
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
