// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Lobby/Stage/ParadiseStageNodeWidget.h"
#include "UI/Data/ParadiseStageItemObject.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UParadiseStageNodeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Enter)
	{
		Btn_Enter->OnClicked.AddDynamic(this, &UParadiseStageNodeWidget::OnClickEnter);
	}
}

#pragma region 인터페이스 구현 (Interface Implementation)

void UParadiseStageNodeWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	CachedData = Cast<UParadiseStageItemObject>(ListItemObject);

	if (CachedData)
	{
		// 1. 텍스트 설정 (FStageStats에서 가져옴)
		if (Text_StageName)
		{
			Text_StageName->SetText(CachedData->StatsData.StageName);
		}

		// 2. 이미지 설정 (FStageAssets에서 가져옴)
		// 주의: 동기 로드는 프레임 드랍 원인이 될 수 있으나, TileView는 화면에 보이는 것만 로드하므로 
		// SoftObjectPtr을 LoadSynchronous로 호출해도 소규모에서는 허용됩니다. 
		// 대규모라면 StreamableManager를 통한 비동기 로드를 권장합니다.
		if (Img_Thumbnail)
		{
			UTexture2D* Tex = CachedData->AssetsData.Thumbnail.LoadSynchronous();
			if (Tex)
			{
				Img_Thumbnail->SetBrushFromTexture(Tex);
			}
		}
	}
}

#pragma endregion 인터페이스 구현 (Interface Implementation)

#pragma region 내부 로직 (Internal Logic)

void UParadiseStageNodeWidget::OnClickEnter()
{
	if (!CachedData) return;

	UE_LOG(LogTemp, Log, TEXT("[StageNode] 선택된 스테이지 ID: %s"), *CachedData->StageID.ToString());

	// TODO: 컨트롤러에게 이 스테이지 ID로 게임 시작 요청
}

#pragma endregion 내부 로직 (Internal Logic)
