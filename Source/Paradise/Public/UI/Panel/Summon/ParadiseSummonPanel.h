// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ParadiseSummonPanel.generated.h"

#pragma region 전방선언
class UButton;
class UImage;
class UTextBlock;
#pragma endregion 전방선언

/**
 * @brief 소환 시스템의 각 탭(캐릭터, 장비 등) 안에 들어갈 컨텐츠 위젯의 기저 클래스
 * @details 이 클래스는 오직 '특정 소환 페이지의 데이터 표시'만 담당합니다.
 */
UCLASS()
class PARADISE_API UParadiseSummonPanel : public UUserWidget
{
	GENERATED_BODY()
	
#pragma region 생명주기
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
#pragma endregion 생명주기

#pragma region 외부 인터페이스
public:
	/**
	 * @brief 패널 데이터를 초기화하거나 갱신하는 함수 (데이터 주도적 설계)
	 * @details 상속받은 자식 클래스에서 구체적인 배너 이미지나 확률 정보를 세팅합니다.
	 */
	virtual void RefreshPanelData();
#pragma endregion 외부 인터페이스

#pragma region 내부 변수
protected:
	/** @brief 소환 수행 버튼 (1회) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_SummonSingle = nullptr;

	/** @brief 소환 수행 버튼 (10회) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_SummonMulti = nullptr;
#pragma endregion 내부 변수

#pragma region 내부 로직
protected:
	UFUNCTION()
	virtual void OnSingleSummonClicked();

	UFUNCTION()
	virtual void OnMultiSummonClicked();
#pragma endregion 내부 로직
};
