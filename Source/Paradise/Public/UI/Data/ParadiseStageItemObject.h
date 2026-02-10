// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Data/Structs/StageStructs.h"
#include "ParadiseStageItemObject.generated.h"

/**
 * @class UParadiseStageItemObject
 * @brief TileView에 데이터를 전달하기 위한 브릿지(Bridge) 객체.
 * @details FStageStats와 FStageAssets 데이터를 하나로 묶어 UI에 전달합니다.
 */
UCLASS()
class PARADISE_API UParadiseStageItemObject : public UObject
{
	GENERATED_BODY()
	
#pragma region 데이터 관리 (Data Management)
public:
	/** @brief 스테이지 고유 ID (RowName) */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FName StageID = NAME_None;

	/** @brief 기획/밸런싱 데이터 (복사본 캐싱) */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FStageStats StatsData;

	/** @brief 아트/리소스 데이터 (복사본 캐싱) */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FStageAssets AssetsData;

	/** * @brief 객체 생성 및 초기화 팩토리 함수
	 * @param Outer 오너 객체
	 * @param InID 스테이지 ID
	 * @param InStats 기획 데이터
	 * @param InAssets 아트 데이터
	 * @return 초기화된 아이템 오브젝트
	 */
	static UParadiseStageItemObject* Create(UObject* Outer, FName InID, const FStageStats& InStats, const FStageAssets& InAssets);
#pragma endregion 데이터 관리 (Data Management)
};
