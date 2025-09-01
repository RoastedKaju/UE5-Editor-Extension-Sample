// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetActions/QuickAssetAction.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "ObjectTools.h"
#include "AssetToolsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "DebugHeader.h"

#define LOCTEXT_NAMESPACE "QuickAssetActions"

void UQuickAssetAction::DuplicateAssets(uint32 NumOfDuplicates)
{
	if (NumOfDuplicates <= 0)
	{
		ShowDialogMessage(EAppMsgType::Ok, "Please Enter a Valid Number", true);
		return;
	}

	const TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	uint32 Counter = 0;

	for (const FAssetData& SelectedAssetData : SelectedAssetsData)
	{
		for (uint32 i = 0; i < NumOfDuplicates; i++)
		{
			const FString SourceAssetPath = SelectedAssetData.GetSoftObjectPath().ToString();
			const FString NewAssetName = SelectedAssetData.AssetName.ToString() + TEXT("_") + FString::FromInt(i + 1);
			const FString NewPath = FPaths::Combine(SelectedAssetData.PackagePath.ToString(), NewAssetName);

			if (UEditorAssetLibrary::DuplicateAsset(SourceAssetPath, NewPath))
			{
				UEditorAssetLibrary::SaveAsset(NewPath, false);
				++Counter;
			}
		}
	}

	if (Counter > 0)
	{
		ShowNotification(FString::FromInt(Counter) + TEXT(" Assets Duplicated!"));
	}
}

void UQuickAssetAction::AddPrefixes()
{
	const TArray<UObject*> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();
	uint32 Counter = 0;

	for (UObject* SelectedAsset : SelectedAssets)
	{
		// Check if asset valid
		if (!SelectedAsset->IsValidLowLevel())
		{
			const FString ErrorMsg = SelectedAsset->GetName() + TEXT(" No Longer Valid.");
			Print(ErrorMsg, FColor::Red);
			continue;
		}

		// Check if prefix is defined for this type
		const FString* Prefix = PrefixMap.Find(SelectedAsset->GetClass());
		if (!Prefix)
		{
			const FString ErrorMsg = SelectedAsset->GetClass()->GetName() + TEXT(" Prefix Not Found, Update your Prefix Map.");
			Print(ErrorMsg, FColor::Red);
			continue;
		}

		// Check if already have the prefix
		if (SelectedAsset->GetName().StartsWith(*Prefix))
		{
			const FString ErrorMsg = SelectedAsset->GetClass()->GetName() + TEXT(" Already has the correct Prefix.");
			Print(ErrorMsg, FColor::Yellow);
			continue;
		}

		// Cache the old name
		FString OldName = SelectedAsset->GetName();
		
		// Material instance
		if (SelectedAsset->IsA<UMaterialInstanceConstant>())
		{
			// Remove "M_" and "_Inst" from asset
			OldName.RemoveFromStart(TEXT("M_"));
			OldName.RemoveFromEnd(TEXT("_Inst"));
		}
		
		const FString NewName = *Prefix + OldName;
		
		UEditorUtilityLibrary::RenameAsset(SelectedAsset,NewName);
		++Counter;
	}

	if (Counter > 0)
	{
		ShowNotification(TEXT("Successfully Renamed " + FString::FromInt(Counter) + " Assets."));
	}
}

void UQuickAssetAction::DeleteUnusedAssets()
{
	const TArray<FAssetData> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssetData();
	TArray<FAssetData> UnusedAssets;

	FixUpRedirectors();

	for (const auto& SelectedAsset : SelectedAssets)
	{
		TArray<FString> AssetReferencers = UEditorAssetLibrary::FindPackageReferencersForAsset(SelectedAsset.GetSoftObjectPath().ToString());

		if (AssetReferencers.IsEmpty())
		{
			UnusedAssets.AddUnique(SelectedAsset);
		}
	}

	if (UnusedAssets.IsEmpty())
	{
		ShowDialogMessage(EAppMsgType::Ok, TEXT("No Unused Assets Found!"), false);
		return;
	}

	const int32 DeletedNumber = ObjectTools::DeleteAssets(UnusedAssets);

	if (DeletedNumber == 0)
	{
		return;
	}

	ShowNotification(TEXT("Successfully Deleted " + FString::FromInt(DeletedNumber) + " Assets."));
}

void UQuickAssetAction::FixUpRedirectors()
{
	TArray<UObjectRedirector*> RedirectorsToFix;

	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	FARFilter Filter;
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Emplace("/Game");
	Filter.ClassPaths.Emplace(FTopLevelAssetPath(UObjectRedirector::StaticClass()->GetPathName()));
	TArray<FAssetData> OutRedirectors;
	
	AssetRegistryModule.Get().GetAssets(Filter, OutRedirectors);

	for (const auto& RedirectorData : OutRedirectors)
	{
		if (UObjectRedirector* RedirectorToFix = Cast<UObjectRedirector>(RedirectorData.GetAsset()))
		{
			RedirectorsToFix.Add(RedirectorToFix);
		}
	}

	const FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
	AssetToolsModule.Get().FixupReferencers(RedirectorsToFix);
}

UQuickAssetAction::UQuickAssetAction()
{
	PrefixMap = {
		{UBlueprint::StaticClass(), "BP_"},
		{UMaterial::StaticClass(), "M_"},
		{UMaterialInstanceConstant::StaticClass(), "MI_"}
	};
}

#undef LOCTEXT_NAMESPACE
