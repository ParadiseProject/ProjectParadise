// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/StreamableManager.h"
#include "Data/Structs/UnitStructs.h"
#include "Data/Structs/ItemStructs.h"
#include "Data/Structs/StageStructs.h"
#include "ParadiseGameInstance.generated.h"

#pragma region 전방 선언
class ULoadingWidget;
#pragma endregion 전방 선언

/**
 * @class UParadiseGameInstance
 * @brief 게임의 수명 주기 동안 유지되는 전역 데이터 및 시스템 관리 클래스.
 * @details
 * 1. 레벨 간 비동기 로딩 (Async Loading) 및 로딩 화면(Loading Screen) 관리. (추후 세이브 관리는 그때 담당자가 추가할 것)
 */
UCLASS()
class PARADISE_API UParadiseGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UParadiseGameInstance();
	virtual void Init() override;

#pragma region 레벨 관리 (Level & Loading System)
public:
	/** @brief 전역 에셋 로더 (비동기 로딩 수행 담당). */
	FStreamableManager StreamableManager;

	/**
	 * @brief 지정된 레벨로 이동하기 전, 필요한 에셋들을 비동기로 로딩합니다.
	 * @details 로딩 위젯을 띄우고, 에셋 로딩이 완료되면 OpenLevel을 호출하여 렉을 방지합니다.
	 * @param LevelName 이동할 레벨의 이름 (예: "L_InGame", "L_Lobby")
	 * @param AssetsToLoad 미리 메모리에 올릴 에셋들의 경로(SoftObjectPtr) 목록
	 */
	UFUNCTION(BlueprintCallable, Category = "Paradise|System|Level")
	void OpenLevelWithAsyncLoad(FName LevelName, const TArray<TSoftObjectPtr<UObject>>& AssetsToLoad);

protected:
	/** @brief 로딩 화면 위젯 클래스 (BP_LoadingWidget 할당). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Paradise|UI")
	TSubclassOf<UUserWidget> LoadingWidgetClass; // ULoadingWidget이 없다면 UUserWidget으로 대체 가능

	/** @brief 최소 로딩 시간 (초). 로딩이 빨라도 이 시간만큼은 연출을 보여줍니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Paradise|Config")
	float MinLoadingTime = 2.0f;

private:
	/** @brief 현재 화면에 떠 있는 로딩 위젯 인스턴스. */
	UPROPERTY()
	TObjectPtr<ULoadingWidget> CurrentLoadingWidget = nullptr;

	/** @brief 이동할 목표 레벨 이름 임시 저장. */
	FName NextLevelName;

	/** @brief 비동기 로딩 핸들. (진행률 확인 및 생명주기 관리용) */
	TSharedPtr<FStreamableHandle> CurrentLoadHandle = nullptr;

	/** @brief 진행률 체크 타이머 핸들. */
	FTimerHandle ProgressTimerHandle;

	/** @brief 로딩 시작 후 경과 시간. */
	float TotalElapsedTime = 0.0f;

	/** @brief 주기적으로 호출되어 로딩 바를 갱신하고 완료를 체크하는 함수. */
	void UpdateLoadingProgress();
#pragma endregion 레벨 관리


#pragma region 데이터 테이블

public:

	/**
	 * @brief [템플릿] 특정 테이블에서 ID로 데이터를 찾아 해당 구조체로 반환하는 함수
	 * @tparam T : 찾고자 하는 구조체 타입 (예: FCharacterStats)
	 * @param Table : 검색할 데이터 테이블 포인터
	 * @param RowName : 찾을 ID
	 * @return 찾은 데이터 포인터 (없으면 nullptr)
	 */
	template <typename T>
	T* GetDataTableRow(UDataTable* Table, FName RowName)
	{
		if (!Table)
		{
			UE_LOG(LogTemp, Error, TEXT("❌ [GameInstance] 테이블이 연결되지 않았습니다!"));
			return nullptr;
		}

		// 언리얼 내부 FindRow 사용 (ContextString은 에러 로그용)
		static const FString ContextString(TEXT("GameInstance_GetData"));
		return Table->FindRow<T>(RowName, ContextString);
	}

public:

	// RequiredAssetDataTags 으로 다른 타입 데이터 테이블 설정하는 위험 X
	//유닛 데이터테이블
	/*영웅(플레이어) 생성에 사용할 에셋 데이터 테이블 */
	UPROPERTY(EditDefaultsOnly, Category = "Squad|Player", meta = (RowType = "CharacterAssets", RequiredAssetDataTags = "RowStructure=/Script/Paradise.CharacterAssets"))
	TObjectPtr<class UDataTable> PlayerAssetsDataTable = nullptr;

	/*영웅(플레이어) 생성에 사용할 스탯 데이터 테이블 */
	UPROPERTY(EditDefaultsOnly, Category = "Squad|Player", meta = (RowType = "CharacterStats", RequiredAssetDataTags = "RowStructure=/Script/Paradise.CharacterStats"))
	TObjectPtr<class UDataTable> PlayerStatsDataTable = nullptr;

	/*적 유닛 생성에 사용할 에셋 데이터 테이블 */
	UPROPERTY(EditDefaultsOnly, Category = "Squad|Player", meta = (RowType = "EnemyAssets", RequiredAssetDataTags = "RowStructure=/Script/Paradise.EnemyAssets"))
	TObjectPtr<class UDataTable> EnemyAssetsDataTable = nullptr;

	/*적 유닛 생성에 사용할 스탯 데이터 테이블 */
	UPROPERTY(EditDefaultsOnly, Category = "Squad|Player", meta = (RowType = "EnemyStats", RequiredAssetDataTags = "RowStructure=/Script/Paradise.EnemyStats"))
	TObjectPtr<class UDataTable> EnemyStatsDataTable = nullptr;

	/*퍼밀리어 생성에 사용할 에셋 데이터 테이블 */
	UPROPERTY(EditDefaultsOnly, Category = "Squad|Player", meta = (RowType = "FamiliarAssets", RequiredAssetDataTags = "RowStructure=/Script/Paradise.FamiliarAssets"))
	TObjectPtr<class UDataTable> FamiliarAssetsDataTable = nullptr;

	/*퍼밀리어 생성에 사용할 스탯 데이터 테이블 */
	UPROPERTY(EditDefaultsOnly, Category = "Squad|Player", meta = (RowType = "FamiliarStats", RequiredAssetDataTags = "RowStructure=/Script/Paradise.FamiliarStats"))
	TObjectPtr<class UDataTable> FamiliarStatsDataTable = nullptr;

	//아이템 , 장비 데이터테이블

	/*방어구 생성에 사용할 스탯 데이터 테이블 */
	UPROPERTY(EditDefaultsOnly, Category = "Squad|Equipment", meta = (RowType = "ArmorAssets", RequiredAssetDataTags = "RowStructure=/Script/Paradise.ArmorAssets"))
	TObjectPtr<class UDataTable> ArmorAssetsDataTable = nullptr;

	/*방어구 생성에 사용할 스탯 데이터 테이블 */
	UPROPERTY(EditDefaultsOnly, Category = "Squad|Equipment", meta = (RowType = "ArmorStats", RequiredAssetDataTags = "RowStructure=/Script/Paradise.ArmorStats"))
	TObjectPtr<class UDataTable> ArmorStatsDataTable = nullptr;

	/*세트보너스에 사용할 스탯 데이터 테이블 */
	UPROPERTY(EditDefaultsOnly, Category = "Squad|Equipment", meta = (RowType = "SetBonusAssets", RequiredAssetDataTags = "RowStructure=/Script/Paradise.SetBonusAssets"))
	TObjectPtr<class UDataTable> SetBonusAssetsDataTable = nullptr;

	/*세트보너스에 사용할 스탯 데이터 테이블 */
	UPROPERTY(EditDefaultsOnly, Category = "Squad|Equipment", meta = (RowType = "SetBonusStats", RequiredAssetDataTags = "RowStructure=/Script/Paradise.SetBonusStats"))
	TObjectPtr<class UDataTable>SetBonusStatsDataTable = nullptr;

	/*무기 생성에 사용할 스탯 데이터 테이블 */
	UPROPERTY(EditDefaultsOnly, Category = "Squad|Equipment", meta = (RowType = "WeaponAssets", RequiredAssetDataTags = "RowStructure=/Script/Paradise.WeaponAssets"))
	TObjectPtr<class UDataTable> WeaponAssetsDataTable = nullptr;

	/*무기 생성에 사용할 스탯 데이터 테이블 */
	UPROPERTY(EditDefaultsOnly, Category = "Squad|Equipment", meta = (RowType = "WeaponStats", RequiredAssetDataTags = "RowStructure=/Script/Paradise.WeaponStats"))
	TObjectPtr<class UDataTable> WeaponStatsDataTable = nullptr;

	//스테이지 데이터 테이블
	UPROPERTY(EditDefaultsOnly, Category = "Squad|Stage", meta = (RowType = "StatgeStats", RequiredAssetDataTags = "RowStructure=/Script/Paradise.StatgeStats"))
	TObjectPtr<class UDataTable> StatgeStatsDataTable = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Squad|Stage", meta = (RowType = "StageAssets", RequiredAssetDataTags = "RowStructure=/Script/Paradise.StageAssets"))
	TObjectPtr<class UDataTable> StageAssetsDataTable = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Squad|Stage", meta = (RowType = "StageWaveDetail", RequiredAssetDataTags = "RowStructure=/Script/Paradise.StageWaveDetail"))
	TObjectPtr<class UDataTable> StageWaveDetailDataTable = nullptr;

private:

#pragma endregion 데이터 테이블

#pragma region 게임 데이터
public:
	/** @brief 현재까지 클리어한 최고 스테이지 인덱스 (0: 1-1 도전 중). */
	UPROPERTY(BlueprintReadWrite, Category = "Paradise|Progress")
	int32 MaxClearedStageIndex = 0;

#pragma endregion 게임 데이터

};
