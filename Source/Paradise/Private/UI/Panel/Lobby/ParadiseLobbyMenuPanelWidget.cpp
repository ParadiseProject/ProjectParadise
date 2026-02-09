// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Panel/Lobby/ParadiseLobbyMenuPanelWidget.h"
#include "Components/Button.h"
#include "Framework/Lobby/LobbyPlayerController.h"

void UParadiseLobbyMenuPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 1. 컨트롤러 캐싱 (매번 GetOwningPlayer를 호출하는 비용 절약)
	CachedController = GetOwningPlayer<ALobbyPlayerController>();
	if (!CachedController)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MenuPanel] Owning Player Controller is NOT ALobbyPlayerController!"));
	}

	// 2. 버튼 델리게이트 바인딩 (안전하게 nullptr 체크 후 연결)
	if (Btn_Battle) Btn_Battle->OnClicked.AddDynamic(this, &UParadiseLobbyMenuPanelWidget::OnClickBattle);
	if (Btn_Summon) Btn_Summon->OnClicked.AddDynamic(this, &UParadiseLobbyMenuPanelWidget::OnClickSummon);
	if (Btn_Squad) Btn_Squad->OnClicked.AddDynamic(this, &UParadiseLobbyMenuPanelWidget::OnClickSquad);
	if (Btn_Enhance) Btn_Enhance->OnClicked.AddDynamic(this, &UParadiseLobbyMenuPanelWidget::OnClickEnhance);
	if (Btn_Codex) Btn_Codex->OnClicked.AddDynamic(this, &UParadiseLobbyMenuPanelWidget::OnClickCodex);
}

#pragma region 로직 구현 (Logic Implementation)

void UParadiseLobbyMenuPanelWidget::RequestMenuChange(EParadiseLobbyMenu InMenu)
{
	// 컨트롤러가 없으면 다시 찾아봄 (방어 코드)
	if (!CachedController)
	{
		CachedController = GetOwningPlayer<ALobbyPlayerController>();
	}

	if (CachedController)
	{
		// 실제 로직은 컨트롤러에게 위임
		CachedController->SetLobbyMenu(InMenu);

		UE_LOG(LogTemp, Log, TEXT("[MenuPanel] 메뉴 버튼 클릭: %d"), (int32)InMenu);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[MenuPanel] 컨트롤러를 찾을 수 없어 메뉴 변경 실패!"));
	}
}

void UParadiseLobbyMenuPanelWidget::OnClickBattle()
{
	RequestMenuChange(EParadiseLobbyMenu::Battle);
}	

void UParadiseLobbyMenuPanelWidget::OnClickSummon()
{
	RequestMenuChange(EParadiseLobbyMenu::Summon);
}

void UParadiseLobbyMenuPanelWidget::OnClickSquad()
{
	RequestMenuChange(EParadiseLobbyMenu::Squad);
}

void UParadiseLobbyMenuPanelWidget::OnClickEnhance()
{
	RequestMenuChange(EParadiseLobbyMenu::Enhance);
}

void UParadiseLobbyMenuPanelWidget::OnClickCodex()
{
	RequestMenuChange(EParadiseLobbyMenu::Codex);
}

#pragma endregion 로직 구현 (Logic Implementation)
