// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseUnit.h"

/**
 * @brief 생성자: 기본 스탯(HP, TeamID)을 초기화합니다.
 */
ABaseUnit::ABaseUnit()
{
    PrimaryActorTick.bCanEverTick = false;

    TeamID = 0;
    MaxHP = 100.f;
    HP = MaxHP;
}

/**
 * @brief 데미지 처리 및 사망 판정을 수행합니다.
 */
float ABaseUnit::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    // 현재 HP에서 데미지만큼 차감
    HP -= ActualDamage;

    UE_LOG(LogTemp, Warning, TEXT("[%s] 데미지 입음! 남은 HP: %.1f"), *GetName(), HP);

    // HP가 0 이하면 파괴
    if (HP <= 0.f)
    {
        UE_LOG(LogTemp, Error, TEXT("[%s] 사망했습니다."), *GetName());
        Destroy();
    }

    return ActualDamage;
}