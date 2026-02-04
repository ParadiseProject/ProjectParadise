// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/DataTable.h"
#include "Data/Structs/StageStructs.h"
#include "InGameGameMode.generated.h"

//1. 게임 상태 정의
UENUM(BlueprintType)
enum class EGamePhase : uint8
{
	Ready,		//스테이지 진입 후 카운트다운(준비)
	Combat,		//본격적인 전투 및 타이머 작동
	Victory,	//승리(보상 지급)
	Defeat,		//패배(타임오버 또는 사망)
	Result		//결과창 표시 후 로비 이동 대기
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

	//게임상태 변경하는 함수
	UFUNCTION(BlueprintCallable, Category = "State")
	void SetGamePhase(EGamePhase NewPhase);

public:
	UFUNCTION(BlueprintCallable, Category = "GameRules")
	void EndStage(bool bIsVictory);

protected:
	//데이터테이블 읽어 스테이지 정보 초기화
	void InitializeStageData(FName StageID);

	//각 단계에 진입할 떄 호출되는 함수들
	void OnPhaseReady();
	void OnPhaseCombat();
	void OnPhaseVictory();
	void OnPhaseDefeat();
	void OnPhaseResult();



protected:
	//에디터에 스테이지 정보 할당 
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	UDataTable* StageInfoTable;

	//캐싱된 게임스테이트 포인터
	UPROPERTY()
	class AInGameGameState* CachedGameState;

	//현재 스테이지 전체 정보
	FStageStats CurrentStageData;

	//현재 게임의 단계(외부에서 판단가능)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	EGamePhase CurrentPhase;
	
};
