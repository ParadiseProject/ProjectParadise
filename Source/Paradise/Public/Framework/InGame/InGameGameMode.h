// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/DataTable.h"
#include "Data/Structs/StageStructs.h"
#include "InGameGameMode.generated.h"

//1. ���� ���� ����
UENUM(BlueprintType)
enum class EGamePhase : uint8
{
	Ready,		// 스테이지 시작 전 카운트다운 (준비 단계)
	Combat,		// 전투 시작 및 타이머 작동 (메인 플레이)
	Victory,	// 승리 (스테이지 클리어)
	Defeat,		// 패배 (타임오버 또는 플레이어 사망)
	Result		// 결과창 표시 및 로비 이동 대기
};

/**
 * 
 */
UCLASS()
class PARADISE_API AInGameGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AInGameGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
public:
	// 게임의 현재 단계를 변경하는 함수 (상태 머신)
	UFUNCTION(BlueprintCallable, Category = "State")
	void SetGamePhase(EGamePhase NewPhase);

public:
	UFUNCTION(BlueprintCallable, Category = "GameRules")
	void EndStage(bool bIsVictory);

protected:
	// 데이터 테이블을 읽어 스테이지 정보를 초기화
	void InitializeStageData(FName StageID);

	// 각 단계(Phase) 진입 시 호출되는 이벤트 함수들
	void OnPhaseReady();
	void OnPhaseCombat();
	void OnPhaseVictory();
	void OnPhaseDefeat();
	void OnPhaseResult();



protected:
	// 에디터에서 할당할 스테이지 정보 데이터 테이블
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	UDataTable* StageInfoTable;

	// 캐싱된 게임 스테이트 참조 (매번 캐스팅 방지)
	UPROPERTY()
	class AInGameGameState* CachedGameState;

	// 현재 로드된 스테이지의 상세 데이터 구조체
	FStageStats CurrentStageData;

	// 현재 게임 진행 단계 (외부에서 확인 가능)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	EGamePhase CurrentPhase;
	
};
