// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Ingame/ParadiseCommonButton.h"

#include "Components/TextBlock.h"

#pragma region 생명주기
void UParadiseCommonButton::NativePreConstruct()
{
	Super::NativePreConstruct();

	// 에디터 미리보기(Preview) 및 초기화 시 텍스트 갱신
	SetButtonText(ButtonLabelText);
}
#pragma endregion 생명주기

#pragma region 외부 인터페이스 구현
void UParadiseCommonButton::SetButtonText(FText InText)
{
	// 내부 데이터 갱신
	ButtonLabelText = InText;

	// 바인딩된 텍스트 위젯이 있다면 실제 UI 업데이트 (캡슐화)
	if (Text_Label)
	{
		Text_Label->SetText(ButtonLabelText);
	}
}
#pragma endregion 외부 인터페이스 구현
