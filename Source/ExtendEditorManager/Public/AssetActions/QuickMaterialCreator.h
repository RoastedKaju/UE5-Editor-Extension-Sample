// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Materials/MaterialExpressionTextureSample.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "SupportedTexturesArray")
	TArray<FString> BaseColorArray = {
		TEXT("_BaseColor"),
		TEXT("_Diffuse"),
		TEXT("_Albedo"),
		TEXT("_Diff")
	};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "SupportedTexturesArray")
	TArray<FString> MetallicArray = {
		TEXT("_Metallic"),
		TEXT("_metal")
	};

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "SupportedTexturesArray")
	TArray<FString> RoughnessArray = {
		TEXT("_Roughness"),
		TEXT("_RoughnessMap"),
		TEXT("_rough")
	};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "SupportedTexturesArray")
	TArray<FString> NormalArray = {
		TEXT("_Normal"),
		TEXT("_NormalMap"),
		TEXT("_nor")
	};
	              
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "SupportedTexturesArray")
	TArray<FString> AmbientOcclusionArray = {
		TEXT("_AmbientOcclusion"),
		TEXT("_AmbientOcclusionMap"),
		TEXT("_AO")
	};

private:
	bool ProcessSelectedData(const TArray<FAssetData>& SelectedData, TArray<UTexture2D*>& OutTextures, FString& PackagePath);
	bool CheckIsNameUsed(const FString& FolderToCheck, const FString& NameToCheck) const;
	UMaterial* CreateMaterialAsset(const FString& InMaterialName, const FString& Path);
	void Default_CreateNewMaterialNode(UMaterial* CreatedMaterial, UTexture2D* SelectedTexture, uint32& PinsConnectedCounter);
	bool TryConnectBaseColor(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* CreatedMaterial);
	bool TryConnectMetallic(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* CreatedMaterial);
	bool TryConnectRoughness(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* CreatedMaterial);
	bool TryConnectNormal(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* CreatedMaterial);
	bool TryConnectAO(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* CreatedMaterial);
	
};
