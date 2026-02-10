// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "CueNotifyCombat.generated.h"

class UFXDataAsset;
/**
 * @brief 모든 전투 이펙트(타격, 스킬 등)를 처리하는 마스터 큐
 */
UCLASS()
class PARADISE_API UCueNotifyCombat : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

public:
    // C++ 로직에서 호출되는 함수
    virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const override;

protected:
    // [중요] 블루프린트에서 이 변수에 DA_GlobalFX 등을 넣어줘야 함
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
    TObjectPtr<UFXDataAsset> FXDataAsset;
};
