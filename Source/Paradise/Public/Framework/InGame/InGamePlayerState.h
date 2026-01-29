// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "InGamePlayerState.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;
class APlayerData;
class UHeroDataAsset; //제대로된 클래스 구현시 변경예정

/**
 * @brief 게임의 전반적인 상태(지휘관 정보)를 관리하는 클래스
 * @details 스쿼드(보유 영웅 3명) 데이터와, 전역 자원(코스트, 골드 등)을 GAS로 관리합니다.
 */
UCLASS()
class PARADISE_API AInGamePlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AInGamePlayerState();

	//  GAS 인터페이스 (지휘관 전용: 코스트/골드 관리)
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }
	UAttributeSet* GetAttributeSet() const { return CommanderAttributeSet; }

	//  스쿼드 관리 (Squad Management)
public:
	/*
	 * @brief 게임 시작 시 스쿼드 데이터를 생성하는 함수
	 * @param StartingHeroes 로비에서 선택한 영웅 데이터 에셋 목록
	 */
	void InitSquad(const TArray<UHeroDataAsset*>& StartingHeroes);

	/*
	 * @brief 인덱스에 해당하는 영웅 데이터(영혼) 반환
	 */
	UFUNCTION(BlueprintCallable, Category = "Squad")
	APlayerData* GetSquadMemberData(int32 Index) const;

	/*
	 * @brief 현재 스                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            쿼드 멤버 수 반환
	 */
	UFUNCTION(BlueprintCallable, Category = "Squad")
	int32 GetSquadSize() const { return SquadMembers.Num(); }

protected:
	/*
	 * @brief 실제 스쿼드 멤버들 (영혼/데이터)
	 * @details 육체(Pawn)가 죽거나 교체되어도 이 데이터는 유지됩니다.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Squad")
	TArray<TObjectPtr<APlayerData>> SquadMembers;

	//  GAS 컴포넌트 (Commander Resources)
	/* 지휘관용 ASC (코스트/쿨타임/패시브 효과 관리용) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	/* 지휘관용 어트리뷰트 (Cost, MaxCost, Gold, RegenRate 등) */
	UPROPERTY()
	TObjectPtr<UAttributeSet> CommanderAttributeSet;
};