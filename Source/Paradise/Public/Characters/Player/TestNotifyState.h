// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "TestNotifyState.generated.h"

/**
 * 
 */
UCLASS()
class PARADISE_API UTestNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

	// 틱마다(매 프레임) 호출됨 (공격 중인 동안 계속 검사)
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
};
