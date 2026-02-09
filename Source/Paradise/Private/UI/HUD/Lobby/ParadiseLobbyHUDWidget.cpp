// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/Lobby/ParadiseLobbyHUDWidget.h"
#include "Framework/Lobby/LobbyPlayerController.h"
#include "Components/WidgetSwitcher.h"

void UParadiseLobbyHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 컨트롤러에 HUD 등록
	if (ALobbyPlayerController* PC = GetOwningPlayer<ALobbyPlayerController>())
	{
		PC->SetLobbyHUD(this);
	}

	// 초기화 시 None(메인 로비) 상태로 시작
	UpdateMenuStats(EParadiseLobbyMenu::None);
}

void UParadiseLobbyHUDWidget::UpdateMenuStats(EParadiseLobbyMenu InCurrentMenu)
{
	if (!Switcher_Content) return;

	// 1. None (메인 로비) 상태 처리
	if (InCurrentMenu == EParadiseLobbyMenu::None)
	{
		Switcher_Content->SetVisibility(ESlateVisibility::Collapsed);

		// 메인 로비로 돌아오면 메뉴 패널(버튼들)은 다시 보여야 함
		//if (WBP_MenuPanel) WBP_MenuPanel->SetVisibility(ESlateVisibility::Visible);

		return;
	}

	// 2. 특정 메뉴 진입 처리
	Switcher_Content->SetVisibility(ESlateVisibility::Visible);

	// 보통 전체 화면 메뉴가 뜨면 우측 버튼들은 가리는 게 깔끔할 수 있음 (기획에 따라 다름)
	// if (WBP_MenuPanel) WBP_MenuPanel->SetVisibility(ESlateVisibility::Collapsed);

	// 3. 위젯 생성 및 교체 (Lazy Loading)
	TObjectPtr<UUserWidget>* FoundWidget = CreatedMenuWidgets.Find(InCurrentMenu);
	if (FoundWidget && *FoundWidget)
	{
		Switcher_Content->SetActiveWidget(*FoundWidget);
	}
	else
	{
		if (TSubclassOf<UUserWidget>* ClassPtr = MenuWidgetClasses.Find(InCurrentMenu))
		{
			if (*ClassPtr)
			{
				UUserWidget* NewWidget = CreateWidget<UUserWidget>(this, *ClassPtr);
				Switcher_Content->AddChild(NewWidget);
				Switcher_Content->SetActiveWidget(NewWidget);
				CreatedMenuWidgets.Add(InCurrentMenu, NewWidget);
			}
		}
	}
}