// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Data/ParadiseStageItemObject.h"

#pragma region 데이터 관리 (Data Management)
UParadiseStageItemObject* UParadiseStageItemObject::Create(UObject* Outer, FName InID, const FStageStats& InStats, const FStageAssets& InAssets)
{
	UParadiseStageItemObject* Obj = NewObject<UParadiseStageItemObject>(Outer);
	if (Obj)
	{
		Obj->StageID = InID;
		Obj->StatsData = InStats;
		Obj->AssetsData = InAssets;
	}
	return Obj;
}
#pragma endregion 데이터 관리 (Data Management)