// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/Core/ParadiseGameInstance.h"
#include "Framework/System/LevelLoadingSubsystem.h"
#include "Kismet/GameplayStatics.h"


UParadiseGameInstance::UParadiseGameInstance()
{
}

void UParadiseGameInstance::Init()
{
	Super::Init();

	// [핵심] 서브시스템에 로딩 위젯 클래스 전달
	if (ULevelLoadingSubsystem* LoadingSystem = GetSubsystem<ULevelLoadingSubsystem>())
	{
		LoadingSystem->SetLoadingWidgetClass(LoadingWidgetClass);
	}

	UE_LOG(LogTemp, Log, TEXT("[ParadiseGameInstance] 초기화 및 로딩 서브시스템 설정 완료."));
}
