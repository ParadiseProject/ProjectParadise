// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Base/PlayerBase.h"
#include "Characters/Player/PlayerData.h"
#include "Components/EquipmentComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Framework/InGame/InGameController.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Data/Enums/GameEnums.h"
#include "Framework/Core/ParadiseGameInstance.h"
#include "Data/Structs/ItemStructs.h"
#include "Kismet/KismetSystemLibrary.h" // 트레이스 함수용
#include "AbilitySystemBlueprintLibrary.h" // GAS 이벤트 전송용

APlayerBase::APlayerBase()
{
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);

    CameraBoom->SetUsingAbsoluteRotation(true); // 캐릭터가 회전해도 카메라는 고정
    CameraBoom->TargetArmLength = 1000.f; // 거리를 멀리 띄움
    CameraBoom->SetRelativeRotation(FRotator(-25.f, -90.f, 0.f)); // 살짝 위에서 아래를 보는 쿼터뷰 각도
    CameraBoom->bDoCollisionTest = false; // 카메라가 벽에 닿아도 줌인되지 않게 (선택사항)

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false; // 카메라는 스프링암만 따라감

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
}

void APlayerBase::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    if (NewController->IsPlayerController())
    {
        // [플레이어 모드]
        // 1. 카메라 활성화
        CameraBoom->SetActive(true);
        FollowCamera->SetActive(true);

        //캐릭터 움직임: 내가 직접 조작
        GetCharacterMovement()->MaxWalkSpeed = 600.f;
    }
    else
    {
        // [AI 모드]
        // 1. 카메라 끄기 (리소스 절약 & 화면 겹침 방지)
        CameraBoom->SetActive(false);
        FollowCamera->SetActive(false);


    }
}

void APlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (IA_Move) {
            EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &APlayerBase::OnMoveInput);
        }
        if (IA_Attack)
        {
            EnhancedInputComponent->BindAction(IA_Attack, ETriggerEvent::Started, this, &APlayerBase::OnAttackInput);
        }
    }
}

void APlayerBase::InitializePlayer(APlayerData* InPlayerData)
{
    if (!InPlayerData) return;

    //연결
    LinkedPlayerData = InPlayerData;
    InPlayerData->CurrentAvatar = this; 

    //GAS 연결
    // Owner(APlayerData): HeroDataActor (데이터/로직의 주체)
    // Avatar(APlayerBase): This Character (애니메이션/물리의 주체)
    UAbilitySystemComponent* ASC = InPlayerData->GetAbilitySystemComponent();
    if (ASC)
    {
        ASC->InitAbilityActorInfo(InPlayerData, this);
    }

    //캐릭터 에셋 외형 업데이트
    //APlayerData의 장비 외형 데이터 테이블의 한줄을 읽어서 외형 업데이트
    if (USkeletalMeshComponent* Mymesh = GetMesh())
    {
        Mymesh->SetSkeletalMesh(LinkedPlayerData->CachedMesh);

        Mymesh->SetAnimInstanceClass(LinkedPlayerData->CachedAnimBP);
    }
    


    //외형 업데이트 (장비 동기화)
    //APlayerData가 가진 장비 컴포넌트를 확인해서 내 몸에 메시를 입힘
    if (UEquipmentComponent* EquipComp = InPlayerData->GetEquipmentComponent())
    {
        //장비컴포넌트에 장착된 장비 비쥬얼적으로 보이게 하는 함수 구현해야함
        //EquipComp->UpdateVisuals(this);
        //UE_LOG(LogTemp, Log, TEXT("💪 [PlayerBase] UpdateVisuals 완료!"));
    }

    UE_LOG(LogTemp, Log, TEXT("💪 [PlayerBase] 육체 초기화 완료!"));
  
	
}

void APlayerBase::CheckHit()
{
    FVector SocketLocation = GetMesh()->GetSocketLocation(TEXT("hand_r")); // 무기 소켓 이름

    // 2. 트레이스 설정 (반경 50cm짜리 구체를 그림)
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(this); // 나는 때리면 안 됨

    FHitResult HitResult;
    bool bHit = UKismetSystemLibrary::SphereTraceSingle(
        GetWorld(),
        SocketLocation,      // 시작점
        SocketLocation,      // 끝점 (제자리에서 구체 검사)
        50.0f,               // 반경 (큐브 크기에 맞춰 조절)
        UEngineTypes::ConvertToTraceType(ECC_Pawn), // 폰(캐릭터)만 검사
        false,               // 복잡한 충돌(Mesh) 말고 단순 캡슐 충돌 검사
        ActorsToIgnore,
        EDrawDebugTrace::ForDuration, // 디버그 선 그리기 (빨간 공 보임)
        HitResult,
        true
    );

    // 3. 무언가 맞았다면?
    if (bHit && HitResult.GetActor())
    {
        AActor* HitActor = HitResult.GetActor();

        // 4. 이미 때린 놈이면 패스 (다단히트 방지)
        if (HitActors.Contains(HitActor)) return;
        HitActors.Add(HitActor); // 목록에 추가

        // 5. [핵심] GAS로 "나 때렸어!" 신호 보내기
        // MeleeBase.cpp에서 기다리는 태그: "Event.Montage.Hit"
        FGameplayEventData Payload;
        Payload.Instigator = this;
        Payload.Target = HitActor;

        // 태그: MeleeBase의 HitEventTag와 똑같아야 함!
        FGameplayTag HitTag = FGameplayTag::RequestGameplayTag(FName("Event.Montage.Hit"));

        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, HitTag, Payload);

        UE_LOG(LogTemp, Warning, TEXT("👊 [PlayerBase] 타격 성공! 대상: %s"), *HitActor->GetName());

    }
}

UAbilitySystemComponent* APlayerBase::GetAbilitySystemComponent() const
{
	return LinkedPlayerData.IsValid() ? LinkedPlayerData->GetAbilitySystemComponent() : nullptr;
}

FCombatActionData APlayerBase::GetCombatActionData(ECombatActionType ActionType) const
{
    FCombatActionData Result;

    // 1. 영혼(PlayerData) 연결 확인
    if (!LinkedPlayerData.IsValid()) return Result;

    // 2. GameInstance 가져오기 (데이터 테이블 검색용)
    UParadiseGameInstance* GI = Cast<UParadiseGameInstance>(GetGameInstance());
    if (!GI) return Result;

    // 3. 장비 컴포넌트 가져오기
    UEquipmentComponent* EquipComp = LinkedPlayerData->GetEquipmentComponent();
    if (!EquipComp) return Result;

    // 4. 현재 장착된 무기 ID 조회 ("Sword_01" 등)
    // (EquipmentComponent.h에 GetEquippedItemID 함수가 있어야 함)
    FName WeaponID = EquipComp->GetEquippedItemID(EEquipmentSlot::Weapon);

    if (WeaponID.IsNone()) return Result; // 무기가 없으면 빈 데이터 반환

    // 5. GI를 통해 데이터 테이블 검색 (ID -> Data Struct)
    FWeaponAssets* Assets = GI->GetDataTableRow<FWeaponAssets>(GI->WeaponAssetsDataTable, WeaponID);
    FWeaponStats* Stats = GI->GetDataTableRow<FWeaponStats>(GI->WeaponStatsDataTable, WeaponID);

    // 6. 데이터가 있으면 구조체에 포장
    if (Assets && Stats)
    {
        // [공통] 데미지 계산 GE 클래스 (독/화염 등 속성 반영)
        Result.DamageEffectClass = Assets->DamageEffectClass;

        // [분기] 행동 타입(평타/스킬)에 따라 다른 데이터 전달
        switch (ActionType)
        {
        case ECombatActionType::BasicAttack:
            // 평타: 기본 몽타주 + 계수 1.0 (평타는 보통 배율 없음)
            Result.MontageToPlay = Assets->BasicAttackMontage.LoadSynchronous();
            Result.DamageMultiplier = 1.0f;
            break;

        case ECombatActionType::WeaponSkill:
            // 스킬: 평타 몽타주(혹은 스킬 몽타주) + 스킬 계수
            // 만약 Assets에 SkillMontage가 있다면 그걸 넣으세요.
            Result.MontageToPlay = Assets->SkillMontage.LoadSynchronous();

            // ★ 핵심: 스탯 테이블에 있는 SkillDamageRate 사용 (1.5배 등)
            Result.DamageMultiplier = Stats->SkillDamageRate;
            break;

        case ECombatActionType::UltimateSkill:
            // 궁극기는 필요 시 CharacterStats에서 가져오도록 확장
            break;
        }
    }

    return Result;
}

void APlayerBase::SetCamera_Default()
{
    if (!CameraBoom || !FollowCamera) return;

    //스프링암 설정
    CameraBoom->TargetArmLength = 2500.0f;
    CameraBoom->SetRelativeRotation(FRotator(-10.0f, -90.0f, 0.0f)); // 거의 수평
    CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 80.0f); // 캐릭터를 화면 하단으로 내림

    //카메라 설정
    FollowCamera->FieldOfView = 30.0f; // 시야각 좁게

    UE_LOG(LogTemp, Log, TEXT("📷 Camera Set: Default"));
}

void APlayerBase::SetCamera_Classic()
{
    if (!CameraBoom || !FollowCamera) return;

    //스프링암 설정
    CameraBoom->TargetArmLength = 1400.0f;
    CameraBoom->SetRelativeRotation(FRotator(-20.0f, -90.0f, 0.0f)); // 위에서 아래로 내려다봄
    CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 50.0f);

    //카메라 설정
    FollowCamera->FieldOfView = 45.0f;

    UE_LOG(LogTemp, Log, TEXT("📷 Camera Set: Classic"));
}

void APlayerBase::SetCamera_Dynamic()
{
    if (!CameraBoom || !FollowCamera) return;

    //스프링암 설정
    CameraBoom->TargetArmLength = 850.0f;
    CameraBoom->SetRelativeRotation(FRotator(-8.0f, -90.0f, 0.0f)); // 로우 앵글 느낌
    CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 60.0f);

    //카메라 설정
    FollowCamera->FieldOfView = 85.0f; 

    UE_LOG(LogTemp, Log, TEXT("📷 Camera Set: Dynamic"));
}

void APlayerBase::SwitchCameraMode()
{
    //다음 인덱스로 변경
    CurrentCameraIndex = (CurrentCameraIndex + 1) % 3;

    //인덱스에 따라 해당 프리셋 적용
    switch (CurrentCameraIndex)
    {
    case 0:
        SetCamera_Default();
        break;
    case 1:
        SetCamera_Classic();
        break;
    case 2:
        SetCamera_Dynamic();
        break;
    default:
        SetCamera_Default(); // 예외 상황 시 기본값
        break;
    }
}


void APlayerBase::Die()
{
    //이미 죽었으면 중복 실행 방지
    if (bIsDead) return;

    UE_LOG(LogTemp, Warning, TEXT("[PlayerBase] 육체가 사망했습니다."));

    //부모의 Die 호출 -> 래그돌(Ragdoll) 실행
    Super::Die();

    //영혼(PlayerData)에게 사망 사실 통보 -> 부활 타이머 가동
    if (LinkedPlayerData.IsValid())
    {
        LinkedPlayerData->OnDeath();
    }

    if (AInGameController* PC = GetWorld()->GetFirstPlayerController<AInGameController>())
    {
        PC->OnPlayerDied(this);
    }

}

void APlayerBase::OnMoveInput(const FInputActionValue& InValue)
{
    //입력값 가져오기 (X: 좌우, Y: 위아래)
    FVector2D MovementVector = InValue.Get<FVector2D>();

    if (!FollowCamera) return;

    FRotator CameraRotation = FollowCamera->GetComponentRotation();
    FRotator YawRotation(0, CameraRotation.Yaw, 0);

    FVector CameraForward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    FVector CameraRight = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);


    //카메라가 보는 깊이 방향으로 (화면상 위아래)
    AddMovementInput(CameraForward, MovementVector.Y);

    //카메라의 오른쪽 방향으로 (화면상 좌우)
    AddMovementInput(CameraRight, MovementVector.X);
}

void APlayerBase::OnAttackInput(const FInputActionValue& InValue)
{
    //일단 기본공격
    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    if (!ASC) return;

    // 2. 공격 태그로 어빌리티 발동 시도
    //FGameplayTag AttackTag = FGameplayTag::RequestGameplayTag(FName("Ability.Type.Skill.Weapon"));
    FGameplayTag AttackTag = FGameplayTag::RequestGameplayTag(FName("Ability.Type.Basic"));

    // 태그로 활성화 (Payload 등은 비워둠)
    ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(AttackTag));
}


