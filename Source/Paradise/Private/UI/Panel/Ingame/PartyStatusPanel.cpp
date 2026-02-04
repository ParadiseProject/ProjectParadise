// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Panel/Ingame/PartyStatusPanel.h"

#include "UI/Widgets/InGame/CharacterStatusWidget.h"

void UPartyStatusPanel::NativeConstruct()
{
	Super::NativeConstruct();

#pragma region 위젯 캐싱 최적화
	// 배열을 미리 비우고 BindWidget된 위젯들을 캐싱하여 루프 처리가 가능하게 함
	MemberWidgets.Empty();

	if (WBP_Member_0) MemberWidgets.Add(WBP_Member_0);
	if (WBP_Member_1) MemberWidgets.Add(WBP_Member_1);
	if (WBP_Member_2) MemberWidgets.Add(WBP_Member_2);
#pragma endregion 위젯 캐싱 최적화
}

#pragma region 데이터 업데이트 로직
void UPartyStatusPanel::InitializeMember(int32 Index, FName CharacterID)
{
	/**
	 * @brief 데이터 드라이븐 방식 적용
	 * @details 실제 프로젝트에서는 DT_CharacterAssets 등에서 CharacterID를 통해
	 * 초상화 텍스처를 가져와 자식 위젯의 SetCharacterPortrait을 호출합니다.
	 */
	if (MemberWidgets.IsValidIndex(Index))
	{
		// 데이터 테이블 로직
		// UTexture2D* Portrait = DataSubsystem->GetPortrait(CharacterID);
		// MemberWidgets[Index]->SetCharacterPortrait(Portrait);

		// 캡슐화: 자식 위젯이 스스로를 초기화하도록 메시지 전달
	}
}
#pragma endregion 데이터 업데이트 로직