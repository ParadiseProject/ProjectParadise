// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interfaces/CombatInterface.h"
#include "CharacterBase.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class APlayerData;

UCLASS()
class PARADISE_API ACharacterBase : public ACharacter, public ICombatInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACharacterBase();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 2. 인터페이스 구현 (가상 함수) -> 자식들이 override 할 필요 없이 여기서 공통 처리
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }

	// 3. 어트리뷰트 셋 Getter (선택 사항이지만 추천)
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	/**
	 * @brief 현재 장착된 무기를 기반으로 특정 행동(평타/스킬)에 필요한 전투 데이터를 반환합니다.
	 * @detail UnitBase와 PlayerBase는 이를 오버라이딩하여 
	 * 서로 다른 데이터 테이블을 참조하여 구조체를 리턴
	 * @return FCombatActionData 몽타주, 데미지 이펙트 클래스, 데미지 계수가 포함된 구조체.
	 */
	virtual FCombatActionData GetCombatActionData(ECombatActionType ActionType) const override { return FCombatActionData();}

	/*
	 * @brief 죽은후 Destroy() 하는 함수
	 * @details 적은 일단 Destory() 변경예정 , 혹은 상위 클래스에서 오버라이드
	 */
	virtual void Die();

	/* @brief 현재 캐릭터의 사망여부 체크 함수 */
	bool IsDead() const { return bIsDead; } // Getter 추가

	/*
	 * @brief 외부에서 생성된 무기 액터를 이 캐릭터의 손(Socket)에 부착하는 함수
	 * @param NewWeapon 부착할 무기 액터
	 * @param SocketName 부착할 소켓 이름
	 */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void AttachWeapon(AActor* NewWeapon, FName SocketName);

	/** * @brief [디버그용] 강제로 Die() 함수를 호출하여 사망 처리 로직을 테스트합니다.
	 * @details 콘솔 명령어(~) 창에서 "TestKillSelf"를 입력하여 실행할 수 있습니다.
	 */
	UFUNCTION(BlueprintCallable, Exec, Category = "Debug")
	void TestKillSelf();

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	
	/*
	 * @brief Hit 이펙트를 적용 시키는 함수 (마테리얼 변화)
	 */
	UFUNCTION(BlueprintCallable)
	void PlayHitFlash();


	/*
	 * @brief Hit 이펙트를 리셋 시키는 함수 (마테리얼 변화)
	*/
	UFUNCTION(BlueprintCallable)
	void ResetHitFlash();


	/* 
	 * @brief DamagePopup(데미지위젯)을 스폰(오브젝트 풀링)시키는 함수  
	 * @param DamageAmount 위젯에 설정할 데미지수치
	 */
	UFUNCTION(BlueprintCallable)
	void SpawnDamagePopup(float DamageAmount);


public:
	/* @brief 피아식별 ID (0: 플레이어, 1: 적, 2: 중립 등)
	 * @details GAS 타겟팅이나 투사체 충돌 처리 시 사용
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	uint8 TeamID = 0;

protected:
	/*
	 * @brief 사망 여부 체크 플래그
	*/
	bool bIsDead = false;

	/*
	 * @brief 머리위 hp위젯컴포넌트
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UWidgetComponent> HealthWidget = nullptr;


	/*
	 * @brief 데미지팝업위젯 액터 할당예정 (클래스타입도 해당 UI타입으로 변경예정)
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DamagePopupActor")
	TSubclassOf<class AActor> DamageTextActorClass = nullptr;

	/*
	 * @brief 실제 무기 액터 인스턴스
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	TObjectPtr<AActor> CurrentWeaponActor = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
private:
		
	/*
	 * @brief 피격 이펙트 타이머 핸들
	*/
	FTimerHandle HitEffectTimerHandle;

	
	/*
	 * @brief 피격 이펙트 리셋 시간 ex) 3초후 리셋
	*/
	float HitResetTime = 0.3f;


};
