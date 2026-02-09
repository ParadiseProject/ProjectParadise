// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "ParadiseStageNodeWidget.generated.h"

#pragma region 전방 선언
class UImage;
class UTextBlock;
class UButton;
class UParadiseStageItemObject;
#pragma endregion 전방 선언

/**
 * @class UParadiseStageNodeWidget
 * @brief TileView 내에서 개별 스테이지(1-1, 1-2)를 표현하는 위젯.
 * @details UserObjectListEntry 인터페이스를 구현하여 데이터를 시각화합니다.
 */
UCLASS()
class PARADISE_API UParadiseStageNodeWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	/** @brief 데이터가 리스트 뷰로부터 전달될 때 호출되는 인터페이스 함수 */
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

#pragma region UI 컴포넌트 (UI Components)
protected:
	/** @brief 스테이지 썸네일 이미지 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_Thumbnail = nullptr;

	/** @brief 스테이지 이름 (예: 1-1 어둠의 숲) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_StageName = nullptr;

	/** @brief 입장 버튼 (전체 영역 클릭) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Enter = nullptr;
#pragma endregion UI 컴포넌트 (UI Components)

#pragma region 내부 로직 (Internal Logic)
private:
	/** @brief 현재 할당된 데이터 객체 캐싱 */
	UPROPERTY()
	TObjectPtr<UParadiseStageItemObject> CachedData = nullptr;

	/** @brief 버튼 클릭 핸들러 */
	UFUNCTION()
	void OnClickEnter();
#pragma endregion 내부 로직 (Internal Logic)
	
};
