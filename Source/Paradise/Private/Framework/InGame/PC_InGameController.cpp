// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/InGame/PC_InGameController.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Characters/Base/PlayerBase.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
void APC_InGameController::BeginPlay()
{
	Super::BeginPlay();
    
	

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        if (DefaultMappingContext)
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    InitializeSquad();
}

void APC_InGameController::SetupInputComponent()
{
	Super::SetupInputComponent();

}

void APC_InGameController::RequestSwitchPlayer(int32 PlayerIndex)
{
	
}

void APC_InGameController::InitializeSquad()
{
}
