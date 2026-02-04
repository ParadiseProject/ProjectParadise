// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Panel/Ingame/ActionControlPanel.h"

#include "CommonButtonBase.h"
#include "UI/Widgets/InGame/SkillSlotWidget.h"

void UActionControlPanel::NativeConstruct()
{
	Super::NativeConstruct();

#pragma region 태그 버튼 배열화 및 캐싱
	// 최적화: 루프 처리를 위해 개별 바인딩된 버튼들을 배열에 담습니다.
	TagButtons.Empty();

	if (TagBtn_A) TagButtons.Add(TagBtn_A);
	if (TagBtn_B) TagButtons.Add(TagBtn_B);
	if (TagBtn_C) TagButtons.Add(TagBtn_C);
#pragma endregion 태그 버튼 배열화 및 캐싱
}

#pragma region 외부 인터페이스 구현
void UActionControlPanel::UpdateSkillCooldown(int32 SkillIndex, float CurrentTime, float MaxTime)
{
	// 기본 스킬 쿨타임 정보 호출
	if (SkillIndex == 0 && SkillSlot_Active)
	{
		SkillSlot_Active->SetCooldownStatus(CurrentTime, MaxTime);
	}
	// 궁극기 쿨타임 정보 호출
	else if (SkillIndex == 1 && SkillSlot_Ultimate)
	{
		SkillSlot_Ultimate->SetCooldownStatus(CurrentTime, MaxTime);
	}
}

void UActionControlPanel::UpdateTagButtons(int32 ActiveCharIndex)
{
	/**
	 * @brief 현재 조작 캐릭터에 따른 태그 버튼 활성화/비활성화 제어
	 * @details 배열 루프를 사용하여 하드코딩 없이 상태를 일괄 업데이트합니다.
	 */
	for (int32 i = 0; i < TagButtons.Num(); ++i)
	{
		if (TagButtons[i])
		{
			// 현재 조작 중인 캐릭터의 버튼은 비활성화(교체 대상에서 제외)
			const bool bIsActiveTag = (i != ActiveCharIndex);
			TagButtons[i]->SetIsEnabled(bIsActiveTag);
		}
	}
}
#pragma endregion 외부 인터페이스 구현