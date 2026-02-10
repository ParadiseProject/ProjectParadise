// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/MeleeBase.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

UMeleeBase::UMeleeBase()
{
	// 기본적으로 감지할 태그 설정
	HitEventTag = FGameplayTag::RequestGameplayTag(FName("Event.Montage.Hit"));
}

void UMeleeBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 코스트 및 쿨타임 확인 (Commit)
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 데이터 가져오기 (인터페이스 사용)
	// BaseGameplayAbility에서 만든 함수가 캐싱된 데이터를 줍니다.
	FCombatActionData CombatData = GetCombatDataFromActor();

	UAnimMontage* MontageToPlay = CombatData.MontageToPlay;

	// 유효성 검사
	if (!MontageToPlay)
	{
		UE_LOG(LogTemp, Warning, TEXT("❌ [MeleeBase] 재생할 몽타주가 없습니다. (ID 확인 필요)"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 몽타주 재생
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		MontageToPlay, // 가져온 몽타주
		1.0f,
		NAME_None,
		false
	);

	// 종료 이벤트 연결
	MontageTask->OnCompleted.AddDynamic(this, &UMeleeBase::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UMeleeBase::OnMontageCompleted);
	MontageTask->OnBlendOut.AddDynamic(this, &UMeleeBase::OnMontageCompleted);
	MontageTask->OnCancelled.AddDynamic(this, &UMeleeBase::OnMontageCompleted);

	MontageTask->ReadyForActivation();

	// 타격 이벤트 대기 (WaitGameplayEvent)
	UAbilityTask_WaitGameplayEvent* EventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		HitEventTag, // "Event.Montage.Hit"
		nullptr,
		false,
		false
	);

	EventTask->EventReceived.AddDynamic(this, &UMeleeBase::OnGameplayEventReceived);
	EventTask->ReadyForActivation();
}

void UMeleeBase::OnGameplayEventReceived(FGameplayEventData Payload)
{
	// 1. 맞은 대상(Target) 확인
	AActor* TargetActor = const_cast<AActor*>(Payload.Target.Get());
	if (!TargetActor) return;

	// 2. 데이터 다시 조회 (Base 클래스에서 캐싱해주므로 비용 걱정 없음)
	FCombatActionData CombatData = GetCombatDataFromActor();

	// GE 클래스가 없으면 데미지 못 줌
	if (!CombatData.DamageEffectClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("⚠️ [MeleeBase] DamageEffectClass가 설정되지 않았습니다."));
		return;
	}

	// 3. GE 스펙 생성 (Make Spec)
	// BaseGameplayAbility에 구현된 Helper 함수 사용
	FGameplayEffectSpecHandle SpecHandle = MakeSpecHandle(CombatData.DamageEffectClass, GetAbilityLevel());

	if (SpecHandle.IsValid())
	{
		// 4. [매우 중요] 데미지 계수 전달 (SetByCaller)
		// 스킬 계수(1.5 등)를 "Data.Damage.Multiplier" 태그로 포장해서 보냅니다.
		// 이 값은 ExecCalcCombat(계산식)에서 꺼내 씁니다.
		SpecHandle.Data->SetSetByCallerMagnitude(
			FGameplayTag::RequestGameplayTag(FName("Data.Damage.Multiplier")),
			CombatData.DamageMultiplier
		);

		// 5. 적용 (Apply)
		ApplySpecHandleToTarget(TargetActor, SpecHandle);
	}
}

void UMeleeBase::OnMontageCompleted()
{
	// [디버깅] 몽타주가 왜 끝났는지 확인
	//UE_LOG(LogTemp, Warning, TEXT("🛑 [MeleeBase] 몽타주 종료됨! 어빌리티 End."));

	// 몽타주가 끝나면 어빌리티 종료
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
