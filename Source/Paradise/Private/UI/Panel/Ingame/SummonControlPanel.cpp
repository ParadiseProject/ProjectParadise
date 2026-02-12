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
	// Data-Driven: 컴포넌트의 데이터 변경사항을 UI에 반영
	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		if (SummonSlots.IsValidIndex(i) && SummonSlots[i])
		{
			// FSummonSlotInfo에 아이콘이 TSoftObjectPtr이라고 가정
			UTexture2D* LoadedIcon = Slots[i].FamiliarIcon.LoadSynchronous();

			// SoldOut 상태라면 아이콘을 null로 처리하거나 별도 로직 적용
			if (Slots[i].bIsSoldOut)
			{
				SummonSlots[i]->UpdateSummonData(nullptr, 0.0f, 0);

				//SummonSlots[i]->RefreshCooldown(3.0f, 3.0f);
			}
			else
			{
				// TODO: 쿨타임 정보는 별도로 관리된다면 인자 수정 필요. 현재는 0.0f로 설정.
				SummonSlots[i]->UpdateSummonData(LoadedIcon, 0.0f, Slots[i].FamiliarCost);
			}
		}
	}
}
#pragma endregion 시스템 초기화

#pragma region 입력 처리
void USummonControlPanel::HandleSlotClickRequest(int32 SlotIndex)
{
	if (CachedSummonComponent.IsValid())
	{
		// 컴포넌트 내부에서 CostManageComponent를 통해 코스트를 차감함
		bool bSuccess = CachedSummonComponent->RequestPurchase(SlotIndex);

		if (bSuccess)
		{
			UE_LOG(LogTemp, Log, TEXT("[SummonPanel] 슬롯 %d 구매 요청 성공"), SlotIndex);
			// 성공 시 UI 업데이트는 HandleSummonSlotsUpdate 델리게이트를 통해 자동으로 처리됨 (Data-Driven)
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[SummonPanel] 슬롯 %d 구매 요청 실패 (코스트 부족 등)"), SlotIndex);
			// 실패 피드백 (예: 흔들림 애니메이션 등) 추가 가능
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[SummonPanel] SummonComponent가 연결되지 않았습니다."));
	}
}
#pragma endregion 입력 처리

#pragma region 외부 인터페이스 구현
void USummonControlPanel::SetSummonSlotData(int32 SlotIndex, UTexture2D* Icon, float MaxCooldown, int32 InCost)
{
	if (SummonSlots.IsValidIndex(SlotIndex) && SummonSlots[SlotIndex])
	{
		SummonSlots[SlotIndex]->UpdateSummonData(Icon, MaxCooldown, InCost);
	}
}

void USummonControlPanel::UpdateSummonCooldown(int32 SlotIndex, float CurrentTime, float MaxTime)
{
	//if (SummonSlots.IsValidIndex(SlotIndex) && SummonSlots[SlotIndex])
	//{
	//	//SummonSlots[SlotIndex]->RefreshCooldown(CurrentTime, MaxTime);
	//}
}
void USummonControlPanel::UpdateCostDisplay(float CurrentCost, float MaxCost)
{
	if (CostWidget)
	{
		CostWidget->UpdateCost(CurrentCost, MaxCost);
	}
}
#pragma endregion 외부 인터페이스 구현
