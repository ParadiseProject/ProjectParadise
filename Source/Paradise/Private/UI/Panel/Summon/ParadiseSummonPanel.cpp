// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Panel/Summon/ParadiseSummonPanel.h"
#include "Components/Button.h"

#pragma region 생명주기
void UParadiseSummonPanel::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_SummonSingle)
	{
		Btn_SummonSingle->OnClicked.AddDynamic(this, &UParadiseSummonPanel::OnSingleSummonClicked);
	}
	if (Btn_SummonMulti)
	{
		Btn_SummonMulti->OnClicked.AddDynamic(this, &UParadiseSummonPanel::OnMultiSummonClicked);
	}
}

void UParadiseSummonPanel::NativeDestruct()
{
	if (Btn_SummonSingle) Btn_SummonSingle->OnClicked.RemoveAll(this);
	if (Btn_SummonMulti) Btn_SummonMulti->OnClicked.RemoveAll(this);

	Super::NativeDestruct();
}
#pragma endregion 생명주기

#pragma region 외부 인터페이스
void UParadiseSummonPanel::RefreshPanelData()
{
	// 자식 클래스에서 오버라이드하여 데이터 테이블 기반으로 배너 이미지 교체 등 수행
	UE_LOG(LogTemp, Log, TEXT("[SummonPanel] 데이터를 갱신합니다."));
}
#pragma endregion 외부 인터페이스

#pragma region 내부 로직
void UParadiseSummonPanel::OnSingleSummonClicked()
{
	// TODO: 가챠 매니저에게 1회 소환 요청
	UE_LOG(LogTemp, Log, TEXT("[SummonPanel] 1회 소환 요청"));
}

void UParadiseSummonPanel::OnMultiSummonClicked()
{
	// TODO: 가챠 매니저에게 10회 소환 요청
	UE_LOG(LogTemp, Log, TEXT("[SummonPanel] 10회 소환 요청"));
}
#pragma endregion 내부 로직