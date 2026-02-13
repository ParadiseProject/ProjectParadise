// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Base/PlayerBase.h"
#include "Characters/Player/PlayerData.h"
#include "Components/EquipmentComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Framework/System/ParadiseSaveGame.h"
#include "Framework/System/InventorySystem.h"
#include "Framework/InGame/InGameController.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/Attributes/BaseAttributeSet.h"
#include "Camera/CameraComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Data/Structs/ItemStructs.h"
#include "Data/Structs/InputStructs.h"
#include "Data/Assets/ParadiseInputConfig.h" 
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

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

    InitializeComponents();

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
}

void APlayerBase::InitializeComponents()
{
    HelmetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HelmetMesh"));
    HelmetMesh->SetupAttachment(GetMesh()); // 부모 메쉬에 붙임
    HelmetMesh->SetLeaderPoseComponent(GetMesh()); // 애니메이션 동기화

    ChestMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ChestMesh"));
    ChestMesh->SetupAttachment(GetMesh());
    ChestMesh->SetLeaderPoseComponent(GetMesh());

    GlovesMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GlovesMesh"));
    GlovesMesh->SetupAttachment(GetMesh());
    GlovesMesh->SetLeaderPoseComponent(GetMesh());

    BootsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BootsMesh"));
    BootsMesh->SetupAttachment(GetMesh());
    BootsMesh->SetLeaderPoseComponent(GetMesh());

    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMeshComp"));
    WeaponMesh->SetupAttachment(GetMesh(), TEXT("hand_r")); // 기본 소켓
    WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 무기 자체 충돌은 끔
    WeaponMesh->SetComponentTickEnabled(false); // 무기 자체 틱은 불필요하므로 끔 (최적화)
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
        if (InputConfig)
        {
            for (const FParadiseInputAction& Action : InputConfig->AbilityInputActions)
            {
                if (Action.InputAction && Action.InputTag != EInputID::None)
                {
                    // 눌렀을 때 (Pressed)
                    EnhancedInputComponent->BindAction(
                        Action.InputAction,
                        ETriggerEvent::Started,
                        this,
                        &APlayerBase::SendAbilityInputToASC, // 배달부 함수 연결
                        Action.InputTag,
                        true
                    );

                    // 뗐을 때 (Released)
                    EnhancedInputComponent->BindAction(
                        Action.InputAction,
                        ETriggerEvent::Completed,
                        this,
                        &APlayerBase::SendAbilityInputToASC,
                        Action.InputTag,
                        false
                    );
                }
            }
        }
    }
}

void APlayerBase::InitializePlayer(APlayerData* InPlayerData)
{
    if (!InPlayerData) return;

    //플레이어 데이터와 플레이어 베이스 와 연결
    LinkedPlayerData = InPlayerData;
    InPlayerData->CurrentAvatar = this;

    // GAS 연결
    // Owner(APlayerData): HeroDataActor (데이터/로직의 주체)
    // Avatar(APlayerBase): This Character (애니메이션/물리의 주체)
    AbilitySystemComponent = InPlayerData->GetAbilitySystemComponent();
    AttributeSet = InPlayerData->GetAttributeSet();

    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(InPlayerData, this);
        UE_LOG(LogTemp, Log, TEXT("💪 [PlayerBase] GAS 초기화 완료!"));
    }

    // 캐릭터 에셋 외형 업데이트
    // APlayerData의 장비 외형 데이터 테이블의 한줄을 읽어서 외형 업데이트
    if (USkeletalMeshComponent* Mymesh = GetMesh())
    {
        Mymesh->SetSkeletalMesh(LinkedPlayerData->CachedMesh);
        Mymesh->SetAnimInstanceClass(LinkedPlayerData->CachedAnimBP);
    }

     // 외형 업데이트, 혹시 모를 데이터 동기화도 다시 (장비 동기화)
    if (UEquipmentComponent* EquipComp = InPlayerData->GetEquipmentComponent())
    {

        EquipComp->UpdateVisuals(this);
        //UGameInstance* GI = GetGameInstance();
        //if (GI)
        //{
        //    //서브시스템 가져오기
        //    if (UInventorySystem* InvSys = GI->GetSubsystem<UInventorySystem>())
        //    {
        //        //데이터 검색
        //        if (const FOwnedCharacterData* CharData = InvSys->GetCharacterDataByID(InPlayerData->CharacterID))
        //        {
        //            //장비 외형 변경 진행
        //            EquipComp->InitializeEquipment(CharData->EquipmentMap);

        //            UE_LOG(LogTemp, Log, TEXT("💪 [PlayerBase] 장비 데이터 연동 및 UpdateVisuals 완료!"));
        //        }
        //    }
        //}
    }

    // 소속 태그 내 몸에 적용
    this->FactionTag = InPlayerData->FactionTag;

    UE_LOG(LogTemp, Log, TEXT("💪 [PlayerBase] 육체 초기화 완료!"));
}

void APlayerBase::BeginPlay()
{
    Super::BeginPlay();
}

USkeletalMeshComponent* APlayerBase::GetArmorComponent(EEquipmentSlot Slot) const
{
    switch (Slot)
    {
    case EEquipmentSlot::Helmet: return HelmetMesh;
    case EEquipmentSlot::Chest:  return ChestMesh;
    case EEquipmentSlot::Gloves: return GlovesMesh;
    case EEquipmentSlot::Boots:  return BootsMesh;
    case EEquipmentSlot::Weapon:  return WeaponMesh;
    default: return nullptr;
    }
}

FCombatActionData APlayerBase::GetCombatActionData(ECombatActionType ActionType) const
{
    if (!LinkedPlayerData.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("⚠️ [PlayerBase] LinkedPlayerData가 유효하지 않음!"));
        return FCombatActionData();
    }

    return LinkedPlayerData->GetCombatActionData(ActionType);
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

void APlayerBase::SendAbilityInputToASC(EInputID InputId, bool bIsPressed)
{
    if (!AbilitySystemComponent) return;;

    if (bIsPressed)
    {
        AbilitySystemComponent->AbilityLocalInputPressed(static_cast<int32>(InputId));
    }
    else
    {
        AbilitySystemComponent->AbilityLocalInputReleased(static_cast<int32>(InputId));
    }
}


