// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Base/PlayerBase.h"
#include "Characters/Player/PlayerData.h"
#include "Components/CMP_Equipment.h"
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


APlayerBase::APlayerBase()
{

}

void APlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        //입력액션 바인딩
    }
}

void APlayerBase::InitializePlayer(APlayerData* InPlayerData)
{
    if (!InPlayerData) return;

    // 1.연결
    LinkedPlayerData = InPlayerData;
    InPlayerData->CurrentAvatar = this; 

    // 2. GAS 연결
    // InitAbilityActorInfo(Owner, Avatar)
    // Owner(주인): HeroDataActor (데이터/로직의 주체)
    // Avatar(표현): This Character (애니메이션/물리의 주체)
    UAbilitySystemComponent* ASC = InPlayerData->GetAbilitySystemComponent();
    if (ASC)
    {
        ASC->InitAbilityActorInfo(InPlayerData, this);
    }

    // 3. 외형 업데이트 (장비 동기화)
    // 영혼이 가진 장비 컴포넌트를 확인해서 내 몸에 메시를 입힘
    if (UCMP_Equipment* EquipComp = InPlayerData->EquipmentComponent)
    {
        //장착된 장비 비쥬얼적으로 보이게 하는 함수
        //EquipComp->UpdateVisualsForPawn(this);
    }

    // 4. 스탯 UI 갱신 (HP바 등)
    // UI 위젯에게 "이제 이 영혼의 ASC를 구독해"라고 알림
    //UpdateHealthBarWidget(ASC);
	
}

UAbilitySystemComponent* APlayerBase::GetAbilitySystemComponent() const
{
	return LinkedPlayerData.IsValid() ? LinkedPlayerData->GetAbilitySystemComponent() : nullptr;
}
