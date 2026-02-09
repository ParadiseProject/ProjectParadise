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

    // =================================================================
    //  1. 메인 로비 (None) 상태
    // =================================================================
    if (InCurrentMenu == EParadiseLobbyMenu::None)
    {
        // 로비 UI 보이기
        if (WBP_MenuPanel) WBP_MenuPanel->SetVisibility(ESlateVisibility::Visible);
        if (WBP_TopBar) WBP_TopBar->SetVisibility(ESlateVisibility::Visible);

        // 콘텐츠(지도 등) 숨기기
        Switcher_Content->SetVisibility(ESlateVisibility::Collapsed);
        return;
    }

    // =================================================================
    //  2. 전투 (Battle) 및 기타 메뉴 상태
    // =================================================================

    // 로비 UI 숨기기
    if (WBP_MenuPanel) WBP_MenuPanel->SetVisibility(ESlateVisibility::Collapsed);

    if (InCurrentMenu == EParadiseLobbyMenu::Battle)
    {
        if (WBP_TopBar) WBP_TopBar->SetVisibility(ESlateVisibility::Collapsed);
    }
    else
    {
        // 다른 메뉴(인벤토리 등)에서는 TopBar가 필요할 수 있음
        if (WBP_TopBar) WBP_TopBar->SetVisibility(ESlateVisibility::Visible);
    }

    // 콘텐츠 스위처 켜기
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
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[HUD] 해당 메뉴(%d)에 연결된 위젯 클래스가 없습니다! BP에서 Config 설정을 확인하세요."), (int32)InCurrentMenu);
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
