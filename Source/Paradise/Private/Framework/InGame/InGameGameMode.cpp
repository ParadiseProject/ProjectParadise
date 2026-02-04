// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/InGame/InGameGameMode.h"
#include "Framework/InGame/InGameGameState.h"

AInGameGameMode::AInGameGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AInGameGameMode::BeginPlay()
{
	Super::BeginPlay();

	//GameState 캐싱
	CachedGameState = GetGameState<AInGameGameState>();

	if (CachedGameState)
	{
		UE_LOG(LogTemp, Log, TEXT("✅ GameState OK!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ GameState NULL"));
	}

	//임시로 1-1 스테이지 정보로 초기화 -> (나중에 GameInstance 연동)
	InitializeStageData(FName("Stage1_1"));

	CurrentPhase = EGamePhase::Result;

	//게임 시작 상태 Ready로 설정
	SetGamePhase(EGamePhase::Ready);
}

void AInGameGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	//UE_LOG(LogTemp, Log, TEXT("Ticking..."));
	if(CurrentPhase == EGamePhase::Combat && CachedGameState && CachedGameState->bIsTimerActive)
	{
		//시간 감소
		CachedGameState->RemainingTime -= DeltaSeconds;
		UE_LOG(LogTemp, Log, TEXT("Time: % f"), CachedGameState->RemainingTime);
		//시간 종료 체크
		if (CachedGameState->RemainingTime <= 0.f)
		{
			CachedGameState->RemainingTime = 0.f;
			CachedGameState->bIsTimerActive = false;
			EndStage(false); //타임오버로 패배 처리
			UE_LOG(LogTemp, Warning, TEXT("시간 초과! 패배 처리 로직 실행"));
		}
	}
}

void AInGameGameMode::SetGamePhase(EGamePhase NewPhase)
{
	if (CurrentPhase == NewPhase) return;

	//상태 변경
	CurrentPhase = NewPhase;

	//상태별 진입 로직
	switch (CurrentPhase)
	{
	case EGamePhase::Ready:
		OnPhaseReady();
		break;
	case EGamePhase::Combat:
		OnPhaseCombat();
		break;
	case EGamePhase::Victory:
		OnPhaseVictory();
		break;
	case EGamePhase::Defeat:
		OnPhaseDefeat();
		break;
	case EGamePhase::Result:
		OnPhaseResult();
		break;
	}
}

void AInGameGameMode::EndStage(bool bIsVictory)
{
	//이미 종료된 상태면 무시
	if (CurrentPhase == EGamePhase::Victory || CurrentPhase == EGamePhase::Defeat) return;

	//승리/패배 상태로 전환
	if (bIsVictory)
	{
		SetGamePhase(EGamePhase::Victory);
	}
	else
	{
		SetGamePhase(EGamePhase::Defeat);
	}
}

void AInGameGameMode::InitializeStageData(FName StageID)
{
	//데이터테이블이 없으면 종료
	if (!StageInfoTable) return;

	//1. 데이터 테이블에서 정보 가져오기
	FStageStats* Row = StageInfoTable->FindRow<FStageStats>(StageID, TEXT("StageInfoContext"));
	if (Row)
	{
		//데이터를 개별변수가 아닌 구조체에 한번에 복사
		CurrentStageData = *Row;

		//2. 게임스태이트에 정보 할당
		if (CachedGameState)
		{
			CachedGameState->DisplayStageName = CurrentStageData.StageName.ToString();
			CachedGameState->RemainingTime = CurrentStageData.TimeLimit;
			CachedGameState->bIsTimerActive = false; 
		}
		//UE_LOG(LogTemp, Log, TEXT("스테이지 로그 완료: %s"), *GS->DisplayStageName);
		UE_LOG(LogTemp, Warning, TEXT("DataLog"));
	}
}

void AInGameGameMode::OnPhaseReady()
{
	UE_LOG(LogTemp, Warning, TEXT("Phase: Ready (3초)"));
	//UE_LOG(LogTemp, Log, TEXT("Phase: Ready (3초 카운트다운)"));

	//1. 카운트다운 UI 표시(게임스테이트 플래그 사용)

	//2. 3초 후 전투 단계(Combat)로 전환
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, [this]()
		{
			SetGamePhase(EGamePhase::Combat);
		}, 3.0f, false);
}


void AInGameGameMode::OnPhaseCombat()
{
	UE_LOG(LogTemp, Log, TEXT("Phase: Combat"));
	//UE_LOG(LogTemp, Log, TEXT("Phase: Combat (전투 시작)"));

	//1. 몬스터스폰 매니저어게 스폰 시작 요청

	if (CachedGameState)
	{
		//2. 타이머 작동시작
		CachedGameState->bIsTimerActive = true;
	}
}

void AInGameGameMode::OnPhaseVictory()
{
	//1. 타이머 정지(GameState의 플래그 사용)
	if (CachedGameState) CachedGameState->bIsTimerActive = false;
	UE_LOG(LogTemp, Log, TEXT("Phase: Victory!"));
	//UE_LOG(LogTemp, Log, TEXT("Phase: Victory! 보상 지급 준비"));

	//2. 보상 지급(게임모드가 관리) 
	int32 RewawrdGold = CurrentStageData.ClearGold;
	int32 ReawardExp = CurrentStageData.ClearExp;
	//GameInstance 호출 로직이 들어가야함

	//3. UI에게 승리연출 
	//UE_LOG(LogTemp, Log, TEXT("승리 판정. 보상 지급 완료"));
}

void AInGameGameMode::OnPhaseDefeat()
{
	if(CachedGameState) CachedGameState->bIsTimerActive = false;
	UE_LOG(LogTemp, Error, TEXT("Phase: Defeat...."));
	//UE_LOG(LogTemp, Error, TEXT("Phase: Defeat.... 보상없음"));

	FTimerHandle ResultTimer;
	GetWorldTimerManager().SetTimer(ResultTimer, [this]() {
		SetGamePhase(EGamePhase::Result);
		}, 3.0f, false);
	
	//패배시 보상 없음
	//UE_LOG(LogTemp, Log, TEXT("패배 판정. 보상 없음"));
}

void AInGameGameMode::OnPhaseResult()
{
	UE_LOG(LogTemp, Log, TEXT("Phase: Result"));
	//UE_LOG(LogTemp, Log, TEXT("Phase: Result (최종 결과 화면 출력)"));
}

