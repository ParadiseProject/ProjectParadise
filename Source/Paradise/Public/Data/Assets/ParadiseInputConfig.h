#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Data/Structs/InputStructs.h"
#include "ParadiseInputConfig.generated.h"

/**
 * @class UParadiseInputConfig
 * @brief 입력 설정 데이터 에셋 (IA와 Enum 매핑 리스트 관리)
 */
UCLASS()
class PARADISE_API UParadiseInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	// 구조체 배열 (에디터에서 목록 편집용)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputAction"))
	TArray<FParadiseInputAction> AbilityInputActions;
};