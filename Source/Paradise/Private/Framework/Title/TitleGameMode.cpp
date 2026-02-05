// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/Title/TitleGameMode.h"
#include "Framework/Title/TitlePlayerController.h"

ATitleGameMode::ATitleGameMode()
{
	// 기본 컨트롤러를 방금 만든 타이틀 전용 컨트롤러로 교체
	PlayerControllerClass = ATitlePlayerController::StaticClass();

	// 타이틀에는 폰(캐릭터)이 필요 없으므로 DefaultPawn을 없앱니다.
	DefaultPawnClass = nullptr;
}