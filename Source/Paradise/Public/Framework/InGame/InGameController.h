// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InGameController.generated.h"


class APlayerBase;
class AAIController;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

/**
 * @brief 인게임 플레이어 컨트롤러
 * @details 영웅 교체(Switching), 스쿼드 명령, UI 인터랙션을 담당합니다.
 */
UCLASS()
class PARADISE_API AInGameController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	//  스쿼드 제어 (Squad Control)
public:
	/*
	 * @brief 요청된 인덱스의 영웅으로 직접 조작 대상을 변경(빙의)하는 함수
	 * @details 기존 영웅에는 AI를 다시 심어주고, 새 영웅의 제어권을 가져옵니다.
	 * @param PlayerIndex 교체할 스쿼드 인덱스 (0 ~ 2)
	 */
	UFUNCTION(BlueprintCallable, Category = "Squad")
	void RequestSwitchPlayer(int32 PlayerIndex);

	/** * @brief 특정 인덱스의 스쿼드 멤버를 부활(재소환) 시킵니다.
	 * @param MemberIndex : 부활시킬 멤버의 인덱스 (0~2)
	 */
	UFUNCTION(BlueprintCallable, Category = "Squad|Command")
	void RespawnSquadPlayer(int32 MemberIndex);

	/** @brief 캐릭터 사망 시 호출되어 다음 생존 캐릭터로 자동 교체합니다. */
	void OnPlayerDied(APlayerBase* DeadPlayer);

private:
	/*
	 * @brief 게임 시작 시 스쿼드 3명을 월드에 스폰하고 초기화하는 함수
	 * @details PlayerState의 데이터를 기반으로 실제 육체(Pawn)를 생성합니다.
	 */
	void InitializeSquadPawns();

	/*
	 * @brief 현재 조종하지 않는 캐릭터에게 AI 컨트롤러를 빙의시키는 함수
	 */
	void PossessAI(APlayerBase* TargetCharacter);


	//  입력 핸들러 (Input Handlers)
private:
	/* 숫자키 1, 2, 3 입력 처리 */
	void OnInputSwitchHero1(const FInputActionValue& Value);
	void OnInputSwitchHero2(const FInputActionValue& Value);
	void OnInputSwitchHero3(const FInputActionValue& Value);

protected:
	//  데이터 및 설정 (Data & Config)
	/*
	 * @brief 현재 필드에 나와 있는 내 영웅들의 육체(Pawn) 캐싱
	 * @details Index 0: 1번 영웅, Index 1: 2번 영웅 ...
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Squad")
	TArray<TObjectPtr<APlayerBase>> ActiveSquadPawns;

	/*
	 * @brief 현재 내가 직접 조종 중인 영웅의 인덱스
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Squad")
	int32 CurrentControlledIndex = 0;

	/*
	 * @brief 동료(AI) 영웅에게 할당할 AI 컨트롤러 클래스
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Squad")
	TSubclassOf<AAIController> SquadAIControllerClass = nullptr;

	//  입력 에셋 (Input Assets)
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext = nullptr;

	/* 영웅 교체 액션 (1, 2, 3번 키) */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_SwitchHero1 = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_SwitchHero2 = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_SwitchHero3 = nullptr;

	/* * @brief 테스트용 캐릭터 클래스 (BP_PlayerBase 할당용)
	 * @details 여기에 에디터에서 만든 캐릭터 블루프린트를 넣어주세요.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Squad|Test")
	TSubclassOf<APlayerBase> TestPlayerClass; // [추가됨]
};