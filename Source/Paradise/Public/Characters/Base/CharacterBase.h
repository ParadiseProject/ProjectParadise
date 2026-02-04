// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"


class APlayerData;
UCLASS()
class PARADISE_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ACharacterBase();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/*
	 * @brief 죽은후 Destroy() 하는 함수
	 * @details 적은 일단 Destory() 변경예정 , 혹은 상위 클래스에서 오버라이드
	 */
	virtual void Die();

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
