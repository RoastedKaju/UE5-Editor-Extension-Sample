// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetActions/QuickMaterialCreator.h"
#include "DebugHeader.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "AssetToolsModule.h"
#include "Factories/MaterialFactoryNew.h"

void UQuickMaterialCreator::CreateMaterialFromSelectedTextures()
{
	if (bUseCustomName)
	{
		if (MaterialName.IsEmpty() || MaterialName.Equals(TEXT("M_")))
		{
			DebugHelper::ShowDialogMessage(EAppMsgType::Ok, TEXT("Please enter a valid name"), false);
			return;
		}
	}

	const TArray<FAssetData> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssetData();
	TArray<UTexture2D*> SelectedTextures;
	FString SelectedTexturePath;
	uint32 PinCounter = 0;

	const bool bSuccess = ProcessSelectedData(SelectedAssets, SelectedTextures, SelectedTexturePath);
	if (!bSuccess) return;

	if (CheckIsNameUsed(SelectedTexturePath, MaterialName))
	{
		return;
	}

	UMaterial* CreatedMaterial = CreateMaterialAsset(MaterialName, SelectedTexturePath);

	if (!IsValid(CreatedMaterial))
	{
		DebugHelper::ShowDialogMessage(EAppMsgType::Ok, TEXT("Failed to create material"), true);
		return;
	}

	for (const auto& Texture : SelectedTextures)
	{
		if (IsValid(Texture))
		{
			Default_CreateNewMaterialNode(CreatedMaterial, Texture, PinCounter);
		}
	}
}

bool UQuickMaterialCreator::ProcessSelectedData(const TArray<FAssetData>& SelectedData, TArray<UTexture2D*>& OutTextures, FString& PackagePath)
{
	if (SelectedData.Num() == 0)
	{
		DebugHelper::ShowDialogMessage(EAppMsgType::Ok, TEXT("No Assets Selected"), true);
		return false;
	}

	bool bMaterialNameSet = false;
	for (const FAssetData& AssetData : SelectedData)
	{
		UObject* SelectedAssetObject = AssetData.GetAsset();

		UTexture2D* SelectedTexture = Cast<UTexture2D>(SelectedAssetObject);

		if (!SelectedTexture)
		{
			DebugHelper::ShowDialogMessage(EAppMsgType::Ok, AssetData.AssetName.ToString() + " Is Not a Texture, select only textures", true);
			return false;
		}

		OutTextures.Add(SelectedTexture);

		if (PackagePath.IsEmpty())
		{
			PackagePath = AssetData.PackagePath.ToString();
		}

		if (!bUseCustomName && !bMaterialNameSet)
		{
			MaterialName = SelectedAssetObject->GetName();
			MaterialName.RemoveFromStart(TEXT("T_"));
			MaterialName.InsertAt(0, TEXT("M_"));
			bMaterialNameSet = true;
		}
	}

	return true;
}

bool UQuickMaterialCreator::CheckIsNameUsed(const FString& FolderToCheck, const FString& NameToCheck) const
{
	const TArray<FString> ListedAssets = UEditorAssetLibrary::ListAssets(FolderToCheck, false);

	for (const auto& Asset : ListedAssets)
	{
		const FString AssetName = FPaths::GetBaseFilename(Asset);

		if (AssetName.Equals(MaterialName))
		{
			DebugHelper::ShowDialogMessage(EAppMsgType::Ok, MaterialName + TEXT(" Name is already in use"), true);
			return true;
		}
	}

	return false;
}

UMaterial* UQuickMaterialCreator::CreateMaterialAsset(const FString& InMaterialName, const FString& Path)
{
	// Load asset tools module
	const FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");

	// Create Asset and Factory
	UMaterialFactoryNew* Factory = NewObject<UMaterialFactoryNew>();
	UObject* CreatedAsset = AssetToolsModule.Get().CreateAsset(InMaterialName, Path, UMaterial::StaticClass(), Factory);

	return Cast<UMaterial>(CreatedAsset);
}

void UQuickMaterialCreator::Default_CreateNewMaterialNode(UMaterial* CreatedMaterial, UTexture2D* SelectedTexture, uint32& PinsConnectedCounter)
{
	UMaterialExpressionTextureSample* TextureSampleNode = NewObject<UMaterialExpressionTextureSample>(CreatedMaterial);

	if (!TextureSampleNode) return;

	if (!CreatedMaterial->HasBaseColorConnected())
	{
		if (TryConnectBaseColor(TextureSampleNode, SelectedTexture, CreatedMaterial))
		{
			PinsConnectedCounter++;
			return;
		}
	}
}

bool UQuickMaterialCreator::TryConnectBaseColor(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* CreatedMaterial)
{
	for (const auto& BaseColorName : BaseColorArray)
	{
		if (SelectedTexture->GetName().Contains(BaseColorName))
		{
			TextureSampleNode->Texture = SelectedTexture;
			UMaterialEditorOnlyData* MaterialEditorData = CreatedMaterial->GetEditorOnlyData();
			if (MaterialEditorData)
			{
				// Add the texture sample node to the material graph
				MaterialEditorData->ExpressionCollection.Expressions.Add(TextureSampleNode);

				// Connect the node to BaseColor
				FExpressionInput& BaseColorInput = CreatedMaterial->GetEditorOnlyData()->BaseColor;
				BaseColorInput.Expression = TextureSampleNode;
			}
			CreatedMaterial->PostEditChange();

			// Offset
			TextureSampleNode->MaterialExpressionEditorX -= 600.0f;

			return true;
		}
	}
	return false;
}
