// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Widgets/Ingame/Result/ResultCharacterSlotWidget.h"
#include "ResultCharacterPanelWidget.generated.h"

#pragma region 전방 선언
class UPanelWidget;
#pragma endregion 전방 선언

/**
 * @class UResultCharacterPanelWidget
 * @brief 결과 화면에서 여러 캐릭터 슬롯(Slot)들을 관리하고 배치하는 컨테이너 위젯.
 * @details
 * 1. 데이터 배열(Array)을 받아서 슬롯 위젯을 동적으로 생성하거나 갱신합니다.
 * 2. 3명을 관리합니다.
 */
UCLASS()
class PARADISE_API UResultCharacterPanelWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

#pragma region 외부 인터페이스
public:
	/**
	 * @brief 캐릭터 데이터 목록을 받아 슬롯들을 생성/갱신합니다.
	 * @param InCharacterDataList 캐릭터 데이터 배열
	 */
	UFUNCTION(BlueprintCallable, Category = "Paradise|UI|Result")
	void UpdateCharacterSlots(const TArray<FResultCharacterData>& InCharacterDataList);
#pragma endregion 외부 인터페이스

#pragma region 설정 (Config)
protected:
	/** @brief 생성할 슬롯 위젯 클래스 (BP_ResultCharacterSlot). */
	UPROPERTY(EditDefaultsOnly, Category = "Paradise|Config")
	TSubclassOf<UResultCharacterSlotWidget> SlotWidgetClass;
#pragma endregion 설정

#pragma region UI 바인딩
protected:
	/** @brief 슬롯들이 실제로 들어갈 컨테이너 (HorizontalBox 등). */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> Container_Slots = nullptr;
#pragma endregion UI 바인딩

#pragma region 레이아웃 설정 (Layout Config)
protected:
	/** @brief 슬롯 간의 간격 및 여백 설정 (기본값: 좌우 20). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Paradise|Layout")
	FMargin SlotPadding = FMargin(20.0f, 0.0f, 20.0f, 0.0f);

	/** @brief 가로 정렬 방식 (Left, Center, Right, Fill). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Paradise|Layout")
	TEnumAsByte<EHorizontalAlignment> SlotHorizontalAlignment = HAlign_Center;

	/** @brief 세로 정렬 방식 (Top, Center, Bottom, Fill). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Paradise|Layout")
	TEnumAsByte<EVerticalAlignment> SlotVerticalAlignment = VAlign_Center;

	/** @brief 크기 정책 (Automatic: 내용물 크기만큼, Fill: 남은 공간 꽉 채우기). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Paradise|Layout")
	TEnumAsByte<ESlateSizeRule::Type> SlotSizeRule = ESlateSizeRule::Automatic;
#pragma endregion 레이아웃 설정
};
