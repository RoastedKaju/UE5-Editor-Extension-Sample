// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetActions/QuickMaterialCreator.h"
#include  "DebugHeader.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"

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

	const bool bSuccess = ProcessSelectedData(SelectedAssets, SelectedTextures, SelectedTexturePath);
	if (!bSuccess) return;

	if (CheckIsNameUsed(SelectedTexturePath, MaterialName))
	{
		return;
	}

	DebugHelper::ShowNotification(SelectedTexturePath);
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
