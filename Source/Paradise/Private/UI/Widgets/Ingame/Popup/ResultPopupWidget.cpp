// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Ingame/Popup/ResultPopupWidget.h"

#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Widget.h"
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
	if (Btn_NextStage) 
	{
		Btn_NextStage->OnClicked.AddUniqueDynamic(this, &UResultPopupWidget::OnNextStageClicked);
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

	// 2. 보상 정보 표시/숨김 처리 (패배 시 보상은 아예 안 보여줌)
	if (Container_RewardInfo)
	{
		Container_RewardInfo->SetVisibility(bIsVictory ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	}

	// 3. 텍스트 갱신 (승리했을 때만 의미 있음)
	if (bIsVictory)
	{
		if (Text_Gold) Text_Gold->SetText(FText::AsNumber(Gold));
		if (Text_Exp) Text_Exp->SetText(FText::AsNumber(Exp));
	}

	// 4. 다음 스테이지 버튼 (승리 시에만 활성화)
	if (Btn_NextStage)
	{
		Btn_NextStage->SetVisibility(bIsVictory ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
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

void UResultPopupWidget::OnNextStageClicked()
{
	// TODO: 다음 스테이지 로딩 로직 (GameInstance나 GameState에 다음 스테이지 ID 저장 필요)
	UE_LOG(LogTemp, Log, TEXT("Move to Next Stage..."));
}
