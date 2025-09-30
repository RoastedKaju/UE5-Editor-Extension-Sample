// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "QuickMaterialCreator.generated.h"

/**
 * 
 */
UCLASS()
class EXTENDEDITORMANAGER_API UQuickMaterialCreator : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void CreateMaterialFromSelectedTextures();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "CreateMaterialFromTextures")
	bool bUseCustomName = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "CreateMaterialFromTextures", meta=(EditCondition="bUseCustomName"))
	FString MaterialName = TEXT("M_");

private:
	bool ProcessSelectedData(const TArray<FAssetData>& SelectedData, TArray<UTexture2D*>& OutTextures, FString& PackagePath);
	bool CheckIsNameUsed(const FString& FolderToCheck, const FString& NameToCheck) const;
};
