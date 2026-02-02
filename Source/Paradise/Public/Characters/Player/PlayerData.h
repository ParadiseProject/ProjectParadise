// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "Engine/DataTable.h"
#include "AbilitySystemInterface.h"
#include "PlayerData.generated.h"

class UAttributeSet;

/**
 * 
 */
UCLASS(Blueprintable)
class PARADISE_API APlayerData : public AInfo ,public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	APlayerData();
	/*
	 * @brief ASC Getter함수
	 * @return ASC 반환
	 */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent()const override { return AbilitySystemComponent; }

	/*
	 * @brief 어트리뷰트셋 Getter함수
	 * @return 어트리뷰트셋 반환
	 */
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	/*
	 * @brief 데이터 테이블 Row 를 통해 초기화하는 함수
	 * @param InDataHandle : 데이터 테이블과 RowName이 담긴 핸들
	 */
	UFUNCTION(BlueprintCallable, Category = "Init")
	void InitFromDataTable(const FDataTableRowHandle& InDataHandle);


	/*
	 * @brief 육체가 사망했을 때 호출되는 함수
	 * @details 부활 타이머를 가동하고 상태를 Dead로 변경합니다.
	 */
	UFUNCTION()
	void OnDeath();

	/*
	 * @brief 리스폰 타이머 끝나면 호출될 함수
	 */
	UFUNCTION()
	void OnRespawnFinished();
public:

	/* * 장비 관리 컴포넌트
	 * @details 인게임에서 착용 중인 장비의 로직과 데이터를 처리합니다.
	 */
	UPROPERTY(VisibleAnywhere, Category = "Equipment")
	TObjectPtr<class UEquipmentComponent> EquipmentComponent = nullptr;


	/* * 현재 빙의 중인 육체 (약한 참조)
	 * @details PlayerBase는 언제든 파괴될 수 있으므로 WeakPtr로 참조합니다.
	 */
	UPROPERTY()
	TWeakObjectPtr<class ACharacterBase> CurrentAvatar = nullptr;

	/* * @brief 캐릭터 데이터 식별자 (Data Table + Row Name)
	 * @details 에디터에서 드롭다운으로 선택 가능합니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (RowType = "CharacterStats"))
	FDataTableRowHandle CharacterDataHandle;
	// meta = (RowType="...")을 넣으면 해당 구조체를 쓰는 테이블만 필터링해서 보여줍니다

	/*
	 * @brief 죽었는지 Bool값
	 */
	UPROPERTY()
	bool bIsDead =false;

	
	/*
	 * @brief 리스폰 대기시간
	 */
	UPROPERTY()
	float RespawnTimer = 5.0f;

protected:
	/*
	 * @brief 플레이어 ASC 컴포넌트
	 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	/* * GAS 스탯 관리용 어트리뷰트 셋
	 * @details [변경 예정] 현재는 기본 UAttributeSet이지만,
	 * 추후 구체적인 커스텀 클래스로 타입을 변경할 예정입니다.
	 */
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

private:

	/*
	 * @brief 리스폰 타이머 핸들
	 */
	FTimerHandle RespawnTimerHandle;

};
