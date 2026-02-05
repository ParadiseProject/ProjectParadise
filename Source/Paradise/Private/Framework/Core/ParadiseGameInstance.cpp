// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/Core/ParadiseGameInstance.h"
#include "UI/Widgets/Loading/LoadingWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"

UParadiseGameInstance::UParadiseGameInstance()
{
}

void UParadiseGameInstance::Init()
{
	Super::Init();
	UE_LOG(LogTemp, Log, TEXT("ParadiseGameInstance 초기화!"));
}

#pragma region 레벨 관리
void UParadiseGameInstance::OpenLevelWithAsyncLoad(FName LevelName, const TArray<TSoftObjectPtr<UObject>>& AssetsToLoad)
{
	if (LevelName.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("ParadiseGameInstance 레벨 이름을 못 찾음!"));
		return;
	}

	NextLevelName = LevelName;
	TotalElapsedTime = 0.0f;

	// 1. 로딩 위젯 생성 및 표시
	if (LoadingWidgetClass)
	{
		// ULoadingWidget으로 바로 생성
		CurrentLoadingWidget = CreateWidget<ULoadingWidget>(this, LoadingWidgetClass);

		if (CurrentLoadingWidget)
		{
			CurrentLoadingWidget->AddToViewport(9999); // 최상위 Z-Order
			CurrentLoadingWidget->SetLoadingPercent(0.0f); // 초기화
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ParadiseGameInstance 로딩 위젯이 설정 안 되어 있음!"));
	}

	// 2. 로딩할 에셋 경로 추출
	TArray<FSoftObjectPath> AssetPaths;
	for (const auto& Asset : AssetsToLoad)
	{
		if (!Asset.IsNull())
		{
			AssetPaths.Add(Asset.ToSoftObjectPath());
		}
	}

	// 3. 비동기 로딩 시작 (에셋이 없어도 로딩 화면 연출을 위해 진행)
	if (AssetPaths.Num() > 0)
	{
		CurrentLoadHandle = StreamableManager.RequestAsyncLoad(AssetPaths);
	}

	// 4. 타이머 시작 (0.1초 간격)
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(ProgressTimerHandle, this, &UParadiseGameInstance::UpdateLoadingProgress, 0.1f, true);
	}

	UE_LOG(LogTemp, Log, TEXT("[ParadiseGameInstance] Start Async Loading for Level: %s"), *LevelName.ToString());
}

void UParadiseGameInstance::UpdateLoadingProgress()
{
	TotalElapsedTime += 0.1f; // 타이머 주기

	// 1. 실제 로딩 진행률 (0.0 ~ 1.0)
	float AssetLoadProgress = 1.0f;
	if (CurrentLoadHandle.IsValid())
	{
		AssetLoadProgress = CurrentLoadHandle->GetProgress();
	}

	// 2. 로딩 위젯 호출
	if (CurrentLoadingWidget)
	{
		CurrentLoadingWidget->SetLoadingPercent(AssetLoadProgress);
	}

	// 3. 로딩 완료 조건 체크
	// (에셋 로딩 완료) AND (최소 로딩 시간 경과)
	if (AssetLoadProgress >= 1.0f && TotalElapsedTime >= MinLoadingTime)
	{
		// 타이머 정지
		GetWorld()->GetTimerManager().ClearTimer(ProgressTimerHandle);
		CurrentLoadHandle.Reset(); // 핸들 해제

		// 로딩 위젯 제거 (선택 사항: OpenLevel 하면 어차피 UI도 날아가지만, 깔끔하게 제거)
		if (CurrentLoadingWidget)
		{
			// 페이드 아웃 애니메이션이 있다면 여기서 실행 후 OpenLevel 호출
			CurrentLoadingWidget->RemoveFromParent();
			CurrentLoadingWidget = nullptr;
		}

		// 4. 레벨 이동
		UGameplayStatics::OpenLevel(this, NextLevelName);
		UE_LOG(LogTemp, Log, TEXT("[ParadiseGameInstance] Open Level: %s"), *NextLevelName.ToString());
	}
}
#pragma endregion 레벨 관리
