// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Ingame/Popup/ResultPopupWidget.h"

#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/Core/ParadiseGameInstance.h"

void UResultPopupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_ToLobby)
	{
		Btn_ToLobby->OnClicked.AddUniqueDynamic(this, &UResultPopupWidget::OnToLobbyClicked);
	}

	if (Btn_Retry)
	{
		Btn_Retry->OnClicked.AddUniqueDynamic(this, &UResultPopupWidget::OnRetryClicked);
	}
}

void UResultPopupWidget::SetResultData(bool bIsVictory, int32 Gold, int32 Exp)
{
	// 1. 타이틀 설정
	if (Text_Title)
	{
		if (bIsVictory)
		{
			Text_Title->SetText(FText::FromString(TEXT("VICTORY")));
			Text_Title->SetColorAndOpacity(FLinearColor::Green);
		}
		else
		{
			Text_Title->SetText(FText::FromString(TEXT("DEFEAT")));
			Text_Title->SetColorAndOpacity(FLinearColor::Red);
		}
	}

	// 2. 보상 설정
	if (Text_Gold) Text_Gold->SetText(FText::AsNumber(Gold));
	if (Text_Exp) Text_Exp->SetText(FText::AsNumber(Exp));
}

void UResultPopupWidget::OnToLobbyClicked()
{
	// GameInstance를 통해 로비로 이동 (비동기 로딩)
	if (UParadiseGameInstance* GI = Cast<UParadiseGameInstance>(GetGameInstance()))
	{
		TArray<TSoftObjectPtr<UObject>> EmptyAssets;
		GI->OpenLevelWithAsyncLoad(FName("CJWTestLobby"), EmptyAssets);
	}
	else
	{
		UGameplayStatics::OpenLevel(this, FName("CJWTestLobby"));
	}
}

void UResultPopupWidget::OnRetryClicked()
{
	// 현재 레벨 재시작
	FString CurrentLevel = UGameplayStatics::GetCurrentLevelName(this);
	UGameplayStatics::OpenLevel(this, FName(*CurrentLevel));
}
