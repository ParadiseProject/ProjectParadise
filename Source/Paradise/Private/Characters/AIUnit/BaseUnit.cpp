// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/AIUnit/BaseUnit.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

/**
 * @brief 생성자: 기본 스탯(HP, TeamID)을 초기화합니다.
 */
ABaseUnit::ABaseUnit()
{
    PrimaryActorTick.bCanEverTick = false;
    TeamID = 0;
}

// 데이터 기반 초기화 함수
void ABaseUnit::InitializeUnit(FCharacterStats* Stats)
{
    if (Stats)
    {
        // 1. 체력 설정
        MaxHP = Stats->BaseMaxHP;
        HP = MaxHP;

        // 2. 공격력 설정
        AttackPower = Stats->BaseAttackPower;

        // 3. 이동 속도 설정 (언리얼 캐릭터 무브먼트 컴포넌트 접근)
        if (GetCharacterMovement())
        {
            GetCharacterMovement()->MaxWalkSpeed = Stats->BaseMoveSpeed;
        }

        UE_LOG(LogTemp, Log, TEXT("%s 초기화: HP %f, 속도 %f"), *GetName(), MaxHP, Stats->BaseMoveSpeed);
    }
}

void ABaseUnit::Die()
{
    // 시각적 비활성화
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
    SetActorTickEnabled(false);

    // AI 제어권은 유지하되 로직만 정지
    AAIController* AIC = Cast<AAIController>(GetController());
    if (AIC && AIC->GetBrainComponent())
    {
        AIC->GetBrainComponent()->StopLogic("Dead");
    }
}

float ABaseUnit::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    HP -= ActualDamage;
    if (HP <= 0)
    {
        Die();
    }
    return ActualDamage;
}