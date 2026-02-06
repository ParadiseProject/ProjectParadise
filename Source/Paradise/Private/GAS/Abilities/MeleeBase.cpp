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

	// 1. 쿨타임 및 비용 지불 확인 (Commit)
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 2. [핵심] 현재 장착된 무기 데이터 가져오기 (Base 함수 사용)
	//FWeaponAssets WeaponData = GetEquippedWeaponAssets();

	//// 데이터 유효성 검사: 몽타주가 없으면 공격 불가
	//if (!WeaponData.AttackMontage)
	//{
	//	// UE_LOG(LogTemp, Warning, TEXT("GA_MeleeBase: Weapon Montage is NULL!"));
	//	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	//	return;
	//}

	// 3. [Task 1] 몽타주 재생 태스크 생성
	//UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
	//	this,
	//	FName("MeleeAttack"),
	//	WeaponData.AttackMontage // 무기에서 가져온 몽타주 사용
	//	// 필요하다면 Rate, SectionName 등 추가 가능
	//);

	// 델리게이트 연결 (끝나면 종료)
	//MontageTask->OnCompleted.AddDynamic(this, &UMeleeBase::OnMontageCompleted);
	//MontageTask->OnInterrupted.AddDynamic(this, &UMeleeBase::OnMontageCompleted);
	//MontageTask->OnCancelled.AddDynamic(this, &UMeleeBase::OnMontageCompleted);

	// 태스크 활성화
	//MontageTask->ReadyForActivation();


	// 4. [Task 2] 타격 이벤트 대기 태스크 생성
	// 애니메이션 노티파이가 보내는 신호를 기다림
	UAbilityTask_WaitGameplayEvent* EventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		HitEventTag,
		nullptr,
		false,
		false
	);

	// 델리게이트 연결 (신호 오면 처리)
	EventTask->EventReceived.AddDynamic(this, &UMeleeBase::OnGameplayEventReceived);

	// 태스크 활성화
	EventTask->ReadyForActivation();
}

void UMeleeBase::OnGameplayEventReceived(FGameplayEventData Payload)
{
	// Payload.Target : 맞은 적 (Actor)
	//AActor* TargetActor = const_cast<AActor*>(Payload.Target);
	//if (!TargetActor) return;

	//// 무기 데이터 다시 조회 (데미지 GE 클래스 가져오기 위함)
	//// (멤버 변수로 캐싱해도 되지만, 함수 호출이 가벼우므로 안전하게 재조회)
	//FWeaponAssets WeaponData = GetEquippedWeaponAssets();

	//if (!WeaponData.DamageEffectClass) return;

	//// Base 클래스의 유틸리티 함수 사용하여 데미지 적용
	//// 1. 택배 포장 (Make Spec)
	//FGameplayEffectSpecHandle SpecHandle = MakeSpecHandle(WeaponData.DamageEffectClass, GetAbilityLevel());

	//// 2. 택배 배송 (Apply)
	//ApplySpecHandleToTarget(TargetActor, SpecHandle);
}

void UMeleeBase::OnMontageCompleted()
{
	// 몽타주가 끝나면 어빌리티도 종료
	bool bReplicateEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}
