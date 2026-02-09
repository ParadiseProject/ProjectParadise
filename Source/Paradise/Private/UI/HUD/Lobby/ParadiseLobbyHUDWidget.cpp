// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/Lobby/ParadiseLobbyHUDWidget.h"
#include "Framework/Lobby/LobbyPlayerController.h"
#include "Components/WidgetSwitcher.h"

#include "UI/Widgets/Lobby/ParadiseLobbyTopBarWidget.h"
#include "UI/Panel/Lobby/ParadiseLobbyMenuPanelWidget.h"

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

	// 0. 공통: 상단 바는 전투 화면 등 특수한 경우를 제외하곤 항상 보여줍니다.
	// (만약 숨겨야 하는 메뉴가 있다면 switch문 안에서 처리)
	if (WBP_TopBar)
	{
		WBP_TopBar->SetVisibility(ESlateVisibility::Visible);
	}

	// 1. None (메인 로비) 상태 처리
	if (InCurrentMenu == EParadiseLobbyMenu::None)
	{
		// 메인 로비: 콘텐츠(지도 등) 끄고, 메뉴 버튼들 켜기
		Switcher_Content->SetVisibility(ESlateVisibility::Collapsed);

		if (WBP_MenuPanel)
		{
			WBP_MenuPanel->SetVisibility(ESlateVisibility::Visible);
		}

		return;
	}

	// 2. 특정 메뉴 진입 처리 (Battle, Summon 등)
	// 메인 메뉴 버튼 가리기 (연출을 위해)
	if (WBP_MenuPanel)
	{
		WBP_MenuPanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	// 콘텐츠 스위처 켜기 (여기에 StageSelect 등이 들어감)
	Switcher_Content->SetVisibility(ESlateVisibility::Visible);

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
				if (NewWidget)
				{
					Switcher_Content->AddChild(NewWidget);
					Switcher_Content->SetActiveWidget(NewWidget);
					CreatedMenuWidgets.Add(InCurrentMenu, NewWidget);
				}
			}
		}
	}
}

void UParadiseLobbyHUDWidget::OnStartCameraMove()
{
	// 상단바, 메뉴 패널, 콘텐츠 스위처 모두 숨김
	if (WBP_TopBar) WBP_TopBar->SetVisibility(ESlateVisibility::Collapsed);
	if (WBP_MenuPanel) WBP_MenuPanel->SetVisibility(ESlateVisibility::Collapsed);
	if (Switcher_Content) Switcher_Content->SetVisibility(ESlateVisibility::Collapsed);

	// 팁: 애니메이션(Fade Out)을 재생하면 더 고급스럽습니다.
}
