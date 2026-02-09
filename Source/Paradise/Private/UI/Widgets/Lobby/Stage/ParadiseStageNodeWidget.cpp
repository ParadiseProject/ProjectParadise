// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Lobby/Stage/ParadiseStageNodeWidget.h"
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

#pragma region 로직 구현
void UParadiseStageNodeWidget::SetupNode(const FStageStats& InStats, const FStageAssets& InAssets)
{
	// 1. 데이터가 들어왔다는 건 해금되었다는 뜻이므로 보이게 설정
	SetVisibility(ESlateVisibility::Visible);

	// 2. 이름 설정
	if (Text_StageName)
	{
		Text_StageName->SetText(InStats.StageName);
	}

	// 3. 썸네일 설정
	if (Img_Thumbnail)
	{
		UTexture2D* Tex = InAssets.Thumbnail.LoadSynchronous();
		if (Tex)
		{
			Img_Thumbnail->SetBrushFromTexture(Tex);
		}
	}

	// TODO: 잠금 여부 처리 (SaveGame 체크 로직 추가 가능)
}

void UParadiseStageNodeWidget::OnClickEnter()
{
	UE_LOG(LogTemp, Log, TEXT("[StageNode] 입장: %s"), *StageID.ToString());
	// 플레이어 컨트롤러에 게임 시작 요청
}
#pragma endregion 로직 구현