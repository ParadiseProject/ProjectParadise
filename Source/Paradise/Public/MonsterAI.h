// Fill out your copyright notice in the Description page of Project Settings.

/**
 * @file MonsterAI.h
 * @brief 몬스터 AI 시스템에서 공통으로 사용되는 상태 정의 및 블랙보드 키 상수 모음
 */

#pragma once

#include "CoreMinimal.h"
#include "MonsterAI.generated.h"

 /**
  * @enum EMonsterState
  * @brief 몬스터의 현재 행동 상태를 나타내는 열거형
  */
UENUM(BlueprintType)
enum class EMonsterState : uint8
{
    Idle        UMETA(DisplayName = "대기"),
    Pursuit     UMETA(DisplayName = "추적"),
    Attack      UMETA(DisplayName = "공격"),
    Dead        UMETA(DisplayName = "사망")
};

/**
 * @namespace BB_KEYS
 * @brief 블랙보드 에셋의 키 이름 오타 방지를 위한 상수 정의 네임스페이스
 */
namespace BB_KEYS
{
    const FName TargetActor = TEXT("TargetActor");
    const FName TargetLocation = TEXT("TargetLocation");
    const FName DistanceToTarget = TEXT("DistanceToTarget");
    const FName AIState = TEXT("AIState");
}