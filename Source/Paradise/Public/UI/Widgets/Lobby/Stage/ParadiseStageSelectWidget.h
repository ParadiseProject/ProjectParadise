// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ParadiseStageSelectWidget.generated.h"

#pragma region 전방 선언
class UCanvasPanel;
class UDataTable;
class UButton;
#pragma endregion 전방 선언

/**
 * @class UParadiseStageSelectWidget
 * @brief 스테이지 선택 화면 메인 위젯.
 * @details 두 개의 데이터 테이블(Stats, Assets)을 읽어 리스트를 초기화합니다.
 */
UCLASS()
class PARADISE_API UParadiseStageSelectWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

#pragma region UI 컴포넌트
protected:
	/** @brief 노드들이 배치된 컨테이너 (지도 배경이 깔린 패널) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> Canvas_MapArea = nullptr;

	/** @brief 로비로 돌아가는 뒤로가기 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Back = nullptr;
#pragma endregion UI 컴포넌트

#pragma region 데이터 (Data)
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Paradise|Data")
	TObjectPtr<UDataTable> DT_StageStats = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Paradise|Data")
	TObjectPtr<UDataTable> DT_StageAssets = nullptr;
#pragma endregion 데이터 (Data)

private:
	/** @brief 뒤로가기 버튼이 눌렸을 때 호출 될 함수 */
	UFUNCTION()
	void OnClickBack();

	/** @brief 맵 노드들의 가시성을 갱신합니다. */
	void RefreshMapNodes();

	/** @brief (임시) 해당 스테이지가 해금되었는지 확인하는 헬퍼 함수 */
	bool IsStageUnlocked(FName StageID);
};
