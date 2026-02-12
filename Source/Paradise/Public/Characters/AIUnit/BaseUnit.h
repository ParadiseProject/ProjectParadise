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

	/** 오브젝트 풀 인터페이스 구현 */
	virtual void OnPoolActivate_Implementation() override;
	virtual void OnPoolDeactivate_Implementation() override;

	/** * @brief 유닛 초기화 함수
	 * FEnemyStats와 FFamiliarStats가 공통으로 상속받는 FAIUnitStats/Assets를 사용하여
	 * 적과 아군 데이터를 모두 수용합니다.
	 */
	void InitializeUnit(struct FAIUnitStats* InStats, struct FAIUnitAssets* InAssets);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void Die();

	UFUNCTION(BlueprintCallable, Category = "Unit|Logic")
	bool IsEnemy(ABaseUnit* OtherUnit);

	void SetUnitID(FName InID) { UnitID = InID; }

	// ID 반환 함수
	FName GetUnitID() const { return UnitID; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit")
	FName UnitID;
};