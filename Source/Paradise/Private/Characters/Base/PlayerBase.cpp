// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Base/PlayerBase.h"
#include "Characters/Player/PlayerData.h"
#include "Components/EquipmentComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


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

        Mymesh->SetAnimClass(LinkedPlayerData->CachedAnimBP);
    }
    


    //외형 업데이트 (장비 동기화)
    //APlayerData가 가진 장비 컴포넌트를 확인해서 내 몸에 메시를 입힘
    if (UEquipmentComponent* EquipComp = InPlayerData->EquipmentComponent2)
    {
        //장비컴포넌트에 장착된 장비 비쥬얼적으로 보이게 하는 함수 구현해야함
        EquipComp->UpdateVisuals(this);
        UE_LOG(LogTemp, Log, TEXT("💪 [PlayerBase] UpdateVisuals 완료!"));
    }

    UE_LOG(LogTemp, Log, TEXT("💪 [PlayerBase] 육체 초기화 완료!"));
  
	
}

UAbilitySystemComponent* APlayerBase::GetAbilitySystemComponent() const
{
	return LinkedPlayerData.IsValid() ? LinkedPlayerData->GetAbilitySystemComponent() : nullptr;
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
    if (LinkedPlayerData.IsValid())
    {
        LinkedPlayerData->OnDeath();
    }

    Super::Die();

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
    //UE_LOG(LogTemp, Log, TEXT("🔥 [Input] 공격 키 눌림! (Attack Input)"));
}


