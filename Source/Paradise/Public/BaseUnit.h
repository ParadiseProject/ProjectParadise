// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseUnit.generated.h"

/**
 * @class ABaseUnit
 * @brief 유닛의 공통 속성(HP, 팀)을 관리하는 베이스 클래스입니다.
 */
UCLASS()
class PARADISE_API ABaseUnit : public ACharacter
{
    GENERATED_BODY()

public:
    ABaseUnit();

    /** @brief 팀 아이디 (0: 아군, 1: 적군) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Stat")
    int32 TeamID;

    /** @brief 현재 체력 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Stat")
    float HP;

    /** @brief 최대 체력 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Stat")
    float MaxHP;

    /**
     * @brief 데미지를 입었을 때 엔진에 의해 호출되는 함수입니다.
     */
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};