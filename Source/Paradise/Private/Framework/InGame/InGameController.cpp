// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/InGame/InGameController.h"
#include "Framework/InGame/InGamePlayerState.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "Characters/Base/PlayerBase.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HUD/Ingame/InGameHUDWidget.h"
#include "Blueprint/UserWidget.h"

void AInGameController::BeginPlay()
{
	Super::BeginPlay();

    //입력 매핑 컨텍스트 연결
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        if (DefaultMappingContext)
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    // [추가] 26/02/04, 담당자: 최지원, [UI 생성] 로컬 플레이어인 경우에만 HUD 생성 (서버/AI 제외)
    if (IsLocalController() && InGameHUDClass)
    {
        // 위젯 생성
        InGameHUDInstance = CreateWidget<UInGameHUDWidget>(this, InGameHUDClass);
        if (InGameHUDInstance)
        {
            // 화면에 부착
            InGameHUDInstance->AddToViewport();

            // HUD 내부 초기화 함수 호출 (연결 고리)
            InGameHUDInstance->InitializeHUD();

            UE_LOG(LogTemp, Log, TEXT("✅ [Controller] InGameHUD 생성 및 초기화 완료"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("❌ [Controller] InGameHUD 생성 실패!"));
        }
    }

    //[더미 테스트] 스쿼드 초기화 요청
    // 실제로는 GameMode나 로비에서 넘겨받은 데이터로 호출해야 함
    AInGamePlayerState* PS = GetPlayerState<AInGamePlayerState>();
    if (PS)
    {
        TArray<FName> TestHeroIDs;
        TestHeroIDs.Add(TEXT("Test1"));
        TestHeroIDs.Add(TEXT("Test2"));
        TestHeroIDs.Add(TEXT("Test3"));
        PS->InitSquad(TestHeroIDs); 

        //육체 소환
        InitializeSquadPawns();
    }

}

void AInGameController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // Enhanced Input 바인딩
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        // 1번 키 -> 인덱스 0
        if (IA_SwitchHero1)
            EnhancedInputComponent->BindAction(IA_SwitchHero1, ETriggerEvent::Triggered, this, &AInGameController::OnInputSwitchHero1);

        // 2번 키 -> 인덱스 1
        if (IA_SwitchHero2)
            EnhancedInputComponent->BindAction(IA_SwitchHero2, ETriggerEvent::Triggered, this, &AInGameController::OnInputSwitchHero2);

        // 3번 키 -> 인덱스 2
        if (IA_SwitchHero3)
            EnhancedInputComponent->BindAction(IA_SwitchHero3, ETriggerEvent::Triggered, this, &AInGameController::OnInputSwitchHero3);
    }

}

void AInGameController::RequestSwitchPlayer(int32 PlayerIndex)
{
    if (!ActiveSquadPawns.IsValidIndex(PlayerIndex))
    {
        UE_LOG(LogTemp, Error, TEXT("❌ [Controller] 잘못된 인덱스 요청: %d"), PlayerIndex);
        return;
    }

    APlayerBase* TargetPawn = ActiveSquadPawns[PlayerIndex];
    if (!TargetPawn || TargetPawn == GetPawn()) return; // 이미 조종 중이면 패스

    // [빙의 실행]
    Possess(TargetPawn);
    CurrentControlledIndex = PlayerIndex;

    // 화면 메시지 출력
    FString Msg = FString::Printf(TEXT("Switch -> Hero %d"), PlayerIndex + 1);
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, Msg);
    UE_LOG(LogTemp, Warning, TEXT("🔄 [Controller] 캐릭터 교체 완료: %d번"), PlayerIndex);
	
}

void AInGameController::InitializeSquadPawns()
{
    AInGamePlayerState* PS = GetPlayerState<AInGamePlayerState>();
    if (!PS) return;

    UE_LOG(LogTemp, Warning, TEXT("🎮 [Controller] 육체(Pawn) 소환 시작..."));

    for (int32 i = 0; i < PS->GetSquadSize(); i++)
    {
        APlayerData* Soul = PS->GetSquadMemberData(i);
        if (Soul)
        {
            FVector SpawnLoc = FVector(0, i * 200.0f, 100.0f);
            FRotator SpawnRot = FRotator::ZeroRotator;

            // [수정됨] 에디터에서 지정한 TestPlayerClass가 있으면 그걸 쓰고, 없으면 기본 C++ 클래스 사용
            UClass* SpawnClass = nullptr;

            if (TestPlayerClass) {
                SpawnClass = TestPlayerClass;
            }
            else {
                SpawnClass = APlayerBase::StaticClass();
            }

            if (!TestPlayerClass)
            {
                UE_LOG(LogTemp, Error, TEXT("⚠️ TestPlayerClass가 설정되지 않았습니다! 입력이 작동하지 않을 수 있습니다."));
            }

            APlayerBase* NewBody = GetWorld()->SpawnActor<APlayerBase>(SpawnClass, SpawnLoc, SpawnRot);

            if (NewBody)
            {
                NewBody->InitializePlayer(Soul);
                ActiveSquadPawns.Add(NewBody);
                DrawDebugString(GetWorld(), SpawnLoc + FVector(0, 0, 100), FString::Printf(TEXT("Squad_%d"), i), nullptr, FColor::Green, -1.0f);
            }
        }
    }

    RequestSwitchPlayer(0);
}

void AInGameController::PossessAI(APlayerBase* TargetCharacter)
{
}

void AInGameController::OnInputSwitchHero1(const FInputActionValue& Value)
{
    //입력 액션 바인딩 함수 후에 UI 모바일 버튼으로 바인딩예정
    RequestSwitchPlayer(0);
}

void AInGameController::OnInputSwitchHero2(const FInputActionValue& Value)
{
    //입력 액션 바인딩 함수 후에 UI 모바일 버튼으로 바인딩예정
    RequestSwitchPlayer(1);
}

void AInGameController::OnInputSwitchHero3(const FInputActionValue& Value)
{
    //입력 액션 바인딩 함수 후에 UI 모바일 버튼으로 바인딩예정
    RequestSwitchPlayer(2);
}


