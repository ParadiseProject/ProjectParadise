// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Data/Structs/UnitStructs.h"
#include "BaseUnit.generated.h"

UCLASS()
class PARADISE_API ABaseUnit : public ACharacter
{
    GENERATED_BODY()

public:
    ABaseUnit();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Stat")
    int32 TeamID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Stat")
    float HP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Stat")
    float MaxHP;

    // 공격력 변수 (데이터테이블에서)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Stat")
    float AttackPower;

    // 데이터테이블 정보를 바탕으로 유닛을 초기화
    void InitializeUnit(struct FCharacterStats* Stats);

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
    void Die();
};