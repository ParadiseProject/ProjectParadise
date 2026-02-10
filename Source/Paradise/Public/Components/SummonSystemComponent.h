// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/Structs/UnitStructs.h"
#include "SummonSystemComponent.generated.h"

class UCostManageComponent;
class UObjectPoolSubsystem;
class AFamiliarUnit;

USTRUCT(BlueprintType)
struct FSummonSlotInfo
{
	GENERATED_BODY()

	/** @brief 데이터 테이블 행 이름 (유닛 ID) */
	UPROPERTY(BlueprintReadOnly, Category = "Summon")
	FName UnitID;

	/** @brief 소환에 책정된 가격 */
	UPROPERTY(BlueprintReadOnly, Category = "Summon")
	int32 FamiliarCost;

	/** @brief UI 아이콘 (Assets 테이블에서 로드해서 UI에 전달) */
	UPROPERTY(BlueprintReadOnly, Category = "Summon")
	TSoftObjectPtr<UTexture2D> UnitIcon; // FUnitBaseAssets에 Icon이 있다고 가정하거나 추가 필요

	/** @brief 빈 슬롯인지 여부 */
	bool bIsEmpty = true;
};

/** @brief 슬롯 정보가 갱신되었을 때 UI에 알리는 델리게이트 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSummonSlotsUpdated, const TArray<FSummonSlotInfo>&, Slots);

/**
 * @class USummonSystemComponent
 * @brief 유닛 소환 시스템을 관리하는 컴포넌트
 * @details
 * - 5개의 랜덤 유닛 슬롯을 관리.
 * - 각 슬롯은 유닛 소환에 필요한 정보(코스트, 쿨타임 등)를 포함.
 * - 각 슬롯의 유닛 가격을 랜덤으로 변동시킴.
 * - CostManageComponent와 연동하여 유닛 소환 시 코스트 차감 및 잔액 확인.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PARADISE_API USummonSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USummonSystemComponent();

protected:
	virtual void BeginPlay() override;

public:	
	/**
	* @brief 슬롯 데이터를 초기화하고 5개를 랜덤으로 채우는 함수
	* @details 게임 시작 시 또는 재시작시 호출함.
	*/
	UFUNCTION(BlueprintCallable, Category = "Summon")
	void RefreshAllSlots();

	/**
	* @brief 특정 슬롯에서 유닛을 소환하는 함수
	* @param SlotIndex 소환할 슬롯의 인덱스 (0~4)
	* @param SpawnLocation 유닛이 소환될 월드 위치
	* @return 소환 성공 여부(돈 부족, 슬롯 비어있음 등 실패시 false)
	*/
	UFUNCTION(BlueprintCallable, Category = "Summon")
	bool TrySummonFamiliar(int32 SlotIndex, FVector SpawnLocation);

protected:
	/** * @brief 실제 유닛을 오브젝트 풀에서 꺼내고 초기화하는 함수 (리팩토링)
	 * @param UnitID 소환할 유닛의 ID (RowName)
	 * @param SpawnLocation 소환 위치
	 * @return 소환 성공 시 유닛 포인터 반환, 실패 시 nullptr
	 */
	//class AFamiliarUnit* SpawnFamiliarUnit(FName UnitID, FVector SpawnLocation);

	/** * @brief 소환 성공 후 뒤처리를 담당하는 내부 함수 (리팩토링)
	 * @param SlotIndex 비워야 할 슬롯 번호
	 */
	void HandleSuccessfulSummon(int32 SlotIndex);

public:
	/** * @brief 특정 슬롯을 다시 채워주는 내부 함수 (타이머에 의해 호출됨)
	 * @param SlotIndex 리필할 슬롯 번호
	 */
	UFUNCTION()
	void RefillSpecificSlot(int32 SlotIndex);

protected:
	/** @brief 현재 슬롯 갱신 델리게이트 */
	UPROPERTY(BlueprintAssignable, Category = "Summon")
	FOnSummonSlotsUpdated OnSlotsUpdated;
	
	/** @brief 슬롯별 리필 타이머 핸들 관리(5개)*/
	FTimerHandle RefillTimers[5];

	/** @brief 랜덤 유닛을 하나 생성하여 슬롯 정보를 반환함 
	* @return 생성된 슬롯 정보 구조체
	*/
	FSummonSlotInfo GenerateRandomSlot();

protected:
	/** @brief 퍼밀리어 스탯 데이터 테이블 */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TObjectPtr<UDataTable> FamiliarStatsTable;

	/** @brief 퍼밀리어 에셋 데이터 테이블 */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TObjectPtr<UDataTable> FamiliarAssetsTable;

	/** @brief 소환할 퍼밀리어 클래스 (BP_FamiliarUnit) */
	//UPROPERTY(EditDefaultsOnly, Category = "Config")
	//TSubclassOf<class AFamiliarUnit> FamiliarClass;

	/** @brief 현재 관리 중인 소환 슬롯들 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	TArray<FSummonSlotInfo> CurrentSlots;

	/** @brief 가격 변동 범위 (예: 0.8 ~ 1.2 배) */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	FVector2D CostRandomRange = FVector2D(0.8f, 1.2f);

	/** @brief 슬롯 자동 갱신 쿨타임 (초 단위) */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float RefillCooldownTime = 3.0f;

	/** @brief 슬롯 개수 (기본 5개) */
	const int32 MaxSlotCount = 5;
};
