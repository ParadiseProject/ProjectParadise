// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Cue/CueNotifyCombat.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

bool UCueNotifyCombat::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	return false;
}
