#pragma once

#include "CoreMinimal.h"
#include "FXStructs.generated.h"

/**
 * @struct FCombatFXSet
 * @brief 시각(VFX)과 청각(SFX) 효과를 하나로 묶은 패키지
 */
USTRUCT(BlueprintType)
struct FCombatFXSet
{
    GENERATED_BODY()

public:
    // 시각 효과 (나이아가라)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
    TSoftObjectPtr<class UNiagaraSystem> VisualEffect;

    // 청각 효과 (사운드)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
    TSoftObjectPtr<class USoundBase> SoundEffect;

    // 크기 조절
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
    FVector Scale = FVector(1.0f);

    // 위치 오프셋 (필요하다면)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
    FVector LocationOffset = FVector::ZeroVector;
};