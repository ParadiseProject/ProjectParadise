// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Ingame/Popup/VictoryPopupWidget.h"
#include "UI/Panel/Ingame/Result/ResultCharacterPanelWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Framework/System/LevelLoadingSubsystem.h"

void UVictoryPopupWidget::NativeConstruct()
{
	Super::NativeConstruct(); // 부모의 버튼(Home, Retry) 바인딩 실행

#pragma region 이벤트 바인딩
	if (Btn_NextStage)
	{
		Btn_NextStage->OnClicked.AddUniqueDynamic(this, &UVictoryPopupWidget::OnNextStageClicked);
	}
#pragma endregion 이벤트 바인딩
}

#pragma region 데이터 설정
void UVictoryPopupWidget::SetVictoryData(FText InStageName, int32 InStarCount, int32 InEarnedGold, int32 InEarnedAether, const TArray<FResultCharacterData>& InCharacterResults)
{
	UE_LOG(LogTemp, Log, TEXT("[VictoryPopup] 데이터 갱신 - 별:%d, 골드:%d, 경험치:%d"), InStarCount, InEarnedGold, InEarnedAether);

	// 1. 텍스트 갱신
	if (Text_Stage)
	{
		Text_Stage->SetText(InStageName);
	}
	if (Text_GoldValue)
	{
		Text_GoldValue->SetText(FText::AsNumber(InEarnedGold));
	}
	if (Text_AetherValue)
	{
		Text_AetherValue->SetText(FText::AsNumber(InEarnedAether));
	}

	// 2. 별 이미지 갱신 (리소스가 설정된 경우에만)
	if (StarOnTexture && StarOffTexture)
	{
		// 삼항 연산자를 사용하여 가독성 확보
		if (Img_Star1) Img_Star1->SetBrushFromTexture(InStarCount >= 1 ? StarOnTexture : StarOffTexture);
		if (Img_Star2) Img_Star2->SetBrushFromTexture(InStarCount >= 2 ? StarOnTexture : StarOffTexture);
		if (Img_Star3) Img_Star3->SetBrushFromTexture(InStarCount >= 3 ? StarOnTexture : StarOffTexture);
	}
	if (WBP_CharacterResultPanel)
	{
		// 팝업은 더 이상 슬롯을 직접 생성하지 않습니다. 패널에게 데이터만 넘깁니다.
		WBP_CharacterResultPanel->UpdateCharacterSlots(InCharacterResults);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[VictoryPopup] WBP_CharacterResultPanel이 바인딩되지 않았습니다."));
	}
}
#pragma endregion 데이터 설정

#pragma region 내부 로직
void UVictoryPopupWidget::OnNextStageClicked()
{
	if (NextStageLevelName.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("[VictoryPopup] 다음 스테이지 이름이 설정되지 않았습니다."));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[VictoryPopup] 다음 스테이지로 이동: %s"), *NextStageLevelName.ToString());

	// 서브시스템을 통한 이동
	if (ULevelLoadingSubsystem* LoadingSystem = GetGameInstance()->GetSubsystem<ULevelLoadingSubsystem>())
	{
		TArray<TSoftObjectPtr<UObject>> EmptyAssets;
		LoadingSystem->StartLevelTransition(NextStageLevelName, FName("L_Loading"), EmptyAssets);
	}
}
#pragma endregion 내부 로직
