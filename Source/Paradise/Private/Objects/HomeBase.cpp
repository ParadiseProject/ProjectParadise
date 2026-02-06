// Fill out your copyright notice in the Description page of Project Settings.

#include "Objects/HomeBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

AHomeBase::AHomeBase()
{
	MaxHP = 500.f;
	HP = MaxHP;
	bIsDead = false;

	PrimaryActorTick.bCanEverTick = false;
}

void AHomeBase::BeginPlay()
{
	Super::BeginPlay();

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->DisableMovement();
		MoveComp->Deactivate();
	}

	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetSimulatePhysics(false);
		Capsule->SetEnableGravity(false);
		Capsule->SetMobility(EComponentMobility::Stationary);
	}
}

void AHomeBase::Die()
{
	UE_LOG(LogTemp, Error, TEXT("🚩 [GAME OVER] %s 가 파괴되었습니다!"), *GetName());

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

}