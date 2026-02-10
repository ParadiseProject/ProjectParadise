// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/Structs/StageStructs.h"
#include "ParadiseStageNodeWidget.generated.h"

#pragma region 전방 선언
class UImage;
class UTextBlock;
class UButton;
class UParadiseStageItemObject;
#pragma endregion 전방 선언

/**
 * @class UStageNodeWidget
 * @brief 월드맵(Canvas)에 수동으로 배치되는 개별 스테이지 노드.
 * @details 에디터에서 직접 배치
 */
UCLASS()
class PARADISE_API UParadiseStageNodeWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

#pragma region 설정 데이터 (Config)
public:
	/** 
	 * @brief 이 노드가 담당할 스테이지 ID (기획자가 에디터에서 직접 입력)
	 * @note "1-1", "1-2"
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FName StageID = NAME_None;
#pragma endregion 설정 데이터 (Config)

#pragma region UI 컴포넌트
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_Thumbnail = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_StageName = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Enter = nullptr;
#pragma endregion UI 컴포넌트

#pragma region 로직 (Logic)
public:
	/**
	 * @brief 부모(StageSelect)로부터 데이터를 받아 UI를 갱신하는 함수.
	 * @param InStats 기획 데이터 (이름 등)
	 * @param InAssets 아트 데이터 (이미지 등)
	 */
	void SetupNode(const FStageStats& InStats, const FStageAssets& InAssets);

private:
	UFUNCTION()
	void OnClickEnter();
#pragma endregion 로직 (Logic)
};
