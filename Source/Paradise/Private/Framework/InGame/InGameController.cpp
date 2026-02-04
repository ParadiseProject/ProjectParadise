// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/InGame/InGameController.h"
#include "Framework/InGame/InGamePlayerState.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "AIController.h"
#include "Characters/Base/PlayerBase.h"
#include "Characters/Player/PlayerData.h"
#include "Kismet/GameplayStatics.h"
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

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), OverviewCameraTag, FoundActors);
    UE_LOG(LogTemp, Warning, TEXT("🔍 [Camera] 태그로 찾은 액터 수: %d개"), FoundActors.Num());
    if (FoundActors.Num() > 0)
    {
        OverviewCameraActor = FoundActors[0];
        UE_LOG(LogTemp, Log, TEXT("✅ [Camera] 태그 '%s'로 카메라 액터(%s)를 찾았습니다."),
            *OverviewCameraTag.ToString(), *OverviewCameraActor->GetName());
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

void AInGameController::SetAutoBattleMode(bool bEnable)
{
    bIsAutoMode = bEnable;

 
    UE_LOG(LogTemp, Warning, TEXT("🤖 [Controller] 자동 전투 모드: %s"), bEnable ? TEXT("ON") : TEXT("OFF"));
    UpdateCameraSystem(); //카메라시점 전체시점으로 변경

    //TODO: 이 아래에 자동모드 AI 전환 함수 구현
}

void AInGameController::RequestSwitchPlayer(int32 PlayerIndex)
{
    if (!ActiveSquadPawns.IsValidIndex(PlayerIndex))
    {
        UE_LOG(LogTemp, Error, TEXT("❌ [Controller] 잘못된 인덱스 요청: %d"), PlayerIndex);
        return;
    }

    APlayerBase* NewPlayer = ActiveSquadPawns[PlayerIndex];
    APlayerBase* OldPlayer = Cast<APlayerBase>(GetPawn());

    //이미 조종 중이거나 대상이 없으면 리턴
    if (!NewPlayer || NewPlayer == OldPlayer) return;
    //죽어있는 플레이어 Base는 리턴
    if (NewPlayer && NewPlayer->IsDead()) return;


    //요청된 캐릭터에 AI가 붙어있었다면 제거
    if (AController* NewPawnController = NewPlayer->GetController())
    {
        // AI 컨트롤러라면 제거
        if (NewPawnController != this)
        {
            NewPawnController->UnPossess();
            NewPawnController->Destroy();
        }
    }

    //요청된 캐릭터로 빙의
    Possess(NewPlayer);
    CurrentControlledIndex = PlayerIndex;

    //이전캐릭터에 AI 주입
    if (OldPlayer)
    {
        PossessAI(OldPlayer);
    }

    // 로그
    FString Msg = FString::Printf(TEXT("Switch -> Hero %d"), PlayerIndex + 1);
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, Msg);

    UE_LOG(LogTemp, Warning, TEXT("🔄 [Controller] 캐릭터 교체 완료 (%s -> %s)"),
        OldPlayer ? *OldPlayer->GetName() : TEXT("None"), // <-- 수정됨
        *NewPlayer->GetName());
	
}

void AInGameController::RespawnSquadPlayer(int32 PlayerIndex)
{
    //유효성 검사
    AInGamePlayerState* PS = GetPlayerState<AInGamePlayerState>();
    if (!PS || !ActiveSquadPawns.IsValidIndex(PlayerIndex))
    {
        UE_LOG(LogTemp, Error, TEXT("❌ [Respawn] 잘못된 인덱스거나 PS가 없습니다."));
        return;
    }

    APlayerData* Soul = PS->GetSquadMemberData(PlayerIndex);
    if (!Soul) return;

    // 이미 살아있는지 확인
    // ActiveSquadPawns[MemberIndex]가 유효하고, IsDead()가 false라면 리턴
    if (ActiveSquadPawns[PlayerIndex] && !ActiveSquadPawns[PlayerIndex]->IsDead())
    {
        UE_LOG(LogTemp, Warning, TEXT("⚠️ [Respawn] 해당 멤버는 이미 살아있습니다."));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("✨ [Respawn] 멤버 %d (%s) 부활 시퀀스 시작!"), PlayerIndex, *Soul->GetName());

    //소환 위치 결정 (현재 조종 중인 캐릭터 주변)
    //위치 수정 예정
    FVector PlayerSpawnLocation = FVector::ZeroVector;
    FRotator PlayerSpawnRotation = FRotator::ZeroRotator;

    if (APawn* LeaderPawn = GetPawn())
    {
        // 내 캐릭터의 뒤쪽 1.5미터, 위로 0.5미터 지점
        PlayerSpawnLocation = LeaderPawn->GetActorLocation() - (LeaderPawn->GetActorForwardVector() * 150.0f) + FVector(0, 0, 50.0f);
        PlayerSpawnRotation = LeaderPawn->GetActorRotation();
    }
    else
    {
        // 전멸 상태라면 PlayerStart 위치 등 기본값 사용
        SpawnLocation = FVector(0, 0, 200.0f);
    }

    //육체(Body) 스폰
    UClass* SpawnClass = nullptr;

    if (TestPlayerClass) {
        SpawnClass = TestPlayerClass;
    }
    else {
        SpawnClass = APlayerBase::StaticClass();
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    APlayerBase* NewBody = GetWorld()->SpawnActor<APlayerBase>(SpawnClass, PlayerSpawnLocation, PlayerSpawnRotation, SpawnParams);

    if (NewBody)
    {
        //데이터 연동 (영혼 주입)
        NewBody->InitializePlayer(Soul);

        //관리 목록 갱신 (죽은 시체 포인터를 새 몸으로 교체)
        ActiveSquadPawns[PlayerIndex] = NewBody;

        //상태 초기화
        Soul->bIsDead = false; // PlayerData에 별도 Setter가 있다면 그걸 사용하세요.

        //전멸상태거나 , 조종중인 Player이 없을때
        if (bIsSquadWipedOut || GetPawn() == nullptr)
        {
            UE_LOG(LogTemp, Warning, TEXT("✨ [Respawn] 전멸 위기에서 %s 부활! 제어권을 획득합니다."), *NewBody->GetName());

            // 전멸 플래그 해제
            bIsSquadWipedOut = false;

            //Possess 및 인덱스 초기화
            Possess(NewBody);
            CurrentControlledIndex = PlayerIndex;

            // 카메라 시점을 이 캐릭터로 갱신
            UpdateCameraSystem();
        }
        else
        {
            // 이미 다른 캐릭터를 조종 중이라면, 부활한 캐릭터는 AI에게 맡김
            if (PlayerIndex != CurrentControlledIndex)
            {
                PossessAI(NewBody);
            }
        }
    }
}

void AInGameController::OnPlayerDied(APlayerBase* DeadPlayer)
{
    bool bIsMyCharacter = false;
    if (ActiveSquadPawns.IsValidIndex(CurrentControlledIndex))
    {
        bIsMyCharacter = (ActiveSquadPawns[CurrentControlledIndex] == DeadPlayer);
    }

    if (bIsMyCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("🚨 [Controller] 플레이어 사망 확인! (Index: %d) -> 다음 생존자 탐색 시작"), CurrentControlledIndex);

        int32 NextAliveIndex = -1;
        int32 SquadSize = ActiveSquadPawns.Num();

        // 현재 인덱스 다음부터 한 바퀴 돌면서 생존 플레이어 탐색
        for (int32 i = 1; i < SquadSize; i++)
        {
            int32 CheckIndex = (CurrentControlledIndex + i) % SquadSize;
            APlayerBase* Candidate = ActiveSquadPawns[CheckIndex];

            // 살아있는 동료 발견! (Candidate가 있고, 죽지 않았어야 함)
            if (Candidate && !Candidate->IsDead())
            {
                NextAliveIndex = CheckIndex;
                break;
            }
        }

        // 생존자가 있으면 교체, 없으면 게임 오버
        if (NextAliveIndex != -1)
        {
            // 바로 교체 요청
            RequestSwitchPlayer(NextAliveIndex);
        }
        else {
            UE_LOG(LogTemp, Error, TEXT("💀 [Controller] 모든 스쿼드 멤버가 사망했습니다."));
            bIsSquadWipedOut = true;
            GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
                {
                    UpdateCameraSystem();
                });
          
        }
    }
    else
    {
        // (AI 동료가 죽은 경우)
        UE_LOG(LogTemp, Warning, TEXT("🤖 [Controller] 동료(AI)가 사망했습니다."));
    }
}

void AInGameController::UpdateCameraSystem()
{
    // 우선순위 1: 전멸했거나 자동 모드일 때 -> Overview 카메라
    if ((bIsSquadWipedOut || bIsAutoMode) && OverviewCameraActor)
    {
        // 현재 타겟이 이미 Overview라면 블렌딩 다시 안 함 (최적화)
        if (GetViewTarget() != OverviewCameraActor)
        {
            SetViewTargetWithBlend(OverviewCameraActor, CameraBlendTime, VTBlend_Cubic);
            UE_LOG(LogTemp, Log, TEXT("📷 [Camera] Overview 모드로 전환"));
        }
    }
    // 우선순위 2: 조종 가능한 캐릭터가 있을 때 -> 캐릭터 카메라
    else if (GetPawn())
    {
        if (GetViewTarget() != GetPawn())
        {
            SetViewTargetWithBlend(GetPawn(), CameraBlendTime, VTBlend_Cubic);
            UE_LOG(LogTemp, Log, TEXT("📷 [Camera] 캐릭터 모드로 복귀"));
        }
    }
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

    for (APlayerBase* Member : ActiveSquadPawns)
    {
        if (Member)
        {
            //일단 전부 AI컨트롤러 Possess
            PossessAI(Member);
        }
    }

    //첫번째 캐릭터에 변경요청
    RequestSwitchPlayer(0);
}

void AInGameController::PossessAI(APlayerBase* TargetCharacter)
{
    if (!TargetCharacter || !SquadAIControllerClass) return;

    //기존 컨트롤러 정리
    if (AController* OldCon = TargetCharacter->GetController())
    {
        //만약 (PlayerController)라면 건드리지 않음
        if (OldCon == this) return;

        OldCon->UnPossess();
        OldCon->Destroy(); // 기존 AI 삭제
    }

    //AI 컨트롤러 스폰
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    //많은 양의 스폰액터가 아니기때문에 오브젝트 풀링 미적용예정
    AAIController* NewAI = GetWorld()->SpawnActor<AAIController>(
        SquadAIControllerClass,
        TargetCharacter->GetActorLocation(),
        TargetCharacter->GetActorRotation(),
        SpawnParams
    );

    if (NewAI)
    {
        //빙의 (OnPossess가 호출되면서 비헤이비어 트리가 실행됨)
        NewAI->Possess(TargetCharacter);
        UE_LOG(LogTemp, Log, TEXT("🤖 [AI] %s에게 AI 컨트롤러가 빙의했습니다."), *TargetCharacter->GetName());
    }
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


