#pragma once

#include "CoreMinimal.h"

/**
 * @enum EParadiseLobbyMenu
 * @brief 로비에서 진입 가능한 메뉴 목록.
 * @details 'Home' 버튼은 제거하고, 아무것도 안 띄운 상태를 None으로 정의.
 */
UENUM(BlueprintType)
enum class EParadiseLobbyMenu : uint8
{
	None        UMETA(DisplayName = "None"),        // 기본 로비 상태 (3D 뷰)
	Battle      UMETA(DisplayName = "Battle"),      // 전투 출정
	Summon      UMETA(DisplayName = "Summon"),      // 소환
	Squad       UMETA(DisplayName = "Squad"),       // 편성
	Enhance     UMETA(DisplayName = "Enhance"),     // 강화
	Codex       UMETA(DisplayName = "Codex")        // 도감
};