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

	virtual void OnPoolActivate_Implementation() override;
	virtual void OnPoolDeactivate_Implementation() override;

	void InitializeUnit(struct FEnemyStats* InStats, struct FEnemyAssets* InAssets);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void Die();

	UFUNCTION(BlueprintCallable, Category = "Unit|Logic")
	bool IsEnemy(ABaseUnit* OtherUnit);
};