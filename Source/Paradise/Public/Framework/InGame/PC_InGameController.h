// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PC_InGameController.generated.h"


class APlayerBase;
class AAIController;
/**
 * 
 */
UCLASS()
class PARADISE_API APC_InGameController : public APlayerController
{
	GENERATED_BODY()
	

public:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	/*
	 * @brief 편성된 플레이어중 요청된 인덱스의 플레이어로 변경하는 함수
	 * \param PlayerIndex 변경할 플레이어 인덱스
	 */
	UFUNCTION(BlueprintCallable, Category = "Squad")
	void RequestSwitchPlayer(int32 PlayerIndex);

private:

	/*
	 * @brief 초기 스쿼드 3명을 스폰하고 세팅하는 함수
	 */
	void InitializeSquad();

protected: 
	/*
	 * @brief 현재 필드에 나와 있는 내 영웅 3명
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Squad")
	TArray<APlayerBase*> ActiveSquad;

	/*
	 * @brief 자동 전투를 담당할 AI 컨트롤러 클래스
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Squad")
	TSubclassOf<AAIController> SquadAIControllerClass = nullptr;

	/*
	 * @brief 입력 매핑 컨텍스트
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Squad")
	TObjectPtr<class UInputMappingContext> DefaultMappingContext = nullptr;


};
