// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "QuickAssetAction.generated.h"

/**
 * 
 */
UCLASS()
class EXTENDEDITORMANAGER_API UQuickAssetAction : public UAssetActionUtility
{
	GENERATED_BODY()

public:
	UFUNCTION(CallInEditor)
	void DuplicateAssets(uint32 NumOfDuplicates);

	UFUNCTION(CallInEditor)
	void AddPrefixes();
	
	UFUNCTION(CallInEditor)
	void DeleteUnusedAssets();

	UFUNCTION()
	void FixUpRedirectors();

protected:
	UQuickAssetAction();
	
private:
	TMap<UClass*, FString> PrefixMap;
};
