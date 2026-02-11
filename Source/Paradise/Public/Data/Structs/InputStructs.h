#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Data/Enums/GameEnums.h"
#include "InputStructs.generated.h"

/**
 * @struct FParadiseInputAction
 * @brief 입력 액션(IA)과 GAS InputID(Enum)를 짝지어주는 구조체
 */
USTRUCT(BlueprintType)
struct FParadiseInputAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EInputID InputTag = EInputID::None;
};
