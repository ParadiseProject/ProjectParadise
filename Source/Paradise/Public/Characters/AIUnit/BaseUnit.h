// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/ObjectPoolInterface.h"
#include "Data/Structs/UnitStructs.h"
#include "GameplayTagContainer.h"
#include "BaseUnit.generated.h"

UCLASS()
class PARADISE_API ABaseUnit : public ACharacter, public IObjectPoolInterface
{
	GENERATED_BODY()

public:
	ABaseUnit();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit|Status")
	float HP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit|Status")
	float MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit|Status")
	FGameplayTag FactionTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit|Status")
	bool bIsDead;

protected:
	/** @brief 데이터 테이블 조회를 위한 RowName 저장 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit|Data")
	FName UnitID;

public:
	/** 오브젝트 풀 인터페이스 구현 */
	virtual void OnPoolActivate_Implementation() override;
	virtual void OnPoolDeactivate_Implementation() override;

	/** @brief 유닛 초기화 및 ID 설정 */
	void InitializeUnit(struct FAIUnitStats* InStats, struct FAIUnitAssets* InAssets);

	void SetUnitID(FName InID) { UnitID = InID; }
	FName GetUnitID() const { return UnitID; }

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Die();

	UFUNCTION(BlueprintCallable, Category = "Unit|Logic")
	bool IsEnemy(ABaseUnit* OtherUnit);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void PlayRangeAttack();
};