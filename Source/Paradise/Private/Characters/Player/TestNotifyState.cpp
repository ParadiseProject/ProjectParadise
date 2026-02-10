// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/TestNotifyState.h"
#include "Characters/Base/PlayerBase.h"

void UTestNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!MeshComp) return;

	// 메시의 주인(캐릭터)를 찾아서
	if (APlayerBase* Player = Cast<APlayerBase>(MeshComp->GetOwner()))
	{
		// "판정 검사해줘!" 라고 요청 (이 함수는 아래에서 만들 겁니다)
		Player->CheckHit();
	}
}
