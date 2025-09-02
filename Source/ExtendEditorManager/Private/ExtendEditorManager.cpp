// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExtendEditorManager.h"
#include "ContentBrowserModule.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "DebugHeader.h"

#define LOCTEXT_NAMESPACE "FExtendEditorManagerModule"

void FExtendEditorManagerModule::StartupModule()
{
	InitContentBrowserExtension();
}

void FExtendEditorManagerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

void FExtendEditorManagerModule::InitContentBrowserExtension()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

	TArray<FContentBrowserMenuExtender_SelectedPaths>& ContentBrowserMenuExtenders = ContentBrowserModule.GetAllPathViewContextMenuExtenders();
	FContentBrowserMenuExtender_SelectedPaths CustomCBMenuDelegate;
	CustomCBMenuDelegate.BindRaw(this, &FExtendEditorManagerModule::CustomCBMenuExtender);

	ContentBrowserMenuExtenders.Add(CustomCBMenuDelegate);
}

TSharedRef<FExtender> FExtendEditorManagerModule::CustomCBMenuExtender(const TArray<FString>& SelectedPaths)
{
	TSharedRef<FExtender> MenuExtender(new FExtender());

	if (!SelectedPaths.IsEmpty())
	{
		FolderPaths = SelectedPaths;
		MenuExtender->AddMenuExtension(FName("Delete"),
		                               EExtensionHook::After,
		                               TSharedPtr<FUICommandList>(),
		                               FMenuExtensionDelegate::CreateRaw(this, &FExtendEditorManagerModule::AddContentBrowserMenuEntry));
	}

	return MenuExtender;
}

void FExtendEditorManagerModule::AddContentBrowserMenuEntry(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Delete Unused Assets")),
		FText::FromString(TEXT("Tooltip for unused asset deletion")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FExtendEditorManagerModule::DeleteUnusedAssets)
	);
}

void FExtendEditorManagerModule::DeleteUnusedAssets()
{
	if (FolderPaths.Num() > 1)
	{
		DebugHelper::ShowDialogMessage(EAppMsgType::Ok, TEXT("You Can Only Do This To One Folder."), true);
		return;
	}

	TArray<FString> AssetsFoundList = UEditorAssetLibrary::ListAssets(FolderPaths[0]);

	if (AssetsFoundList.IsEmpty())
	{
		DebugHelper::ShowDialogMessage(EAppMsgType::Ok, TEXT("There are no assets found."), true);
		return;
	}

	const EAppReturnType::Type ConfirmResult = DebugHelper::ShowDialogMessage(EAppMsgType::YesNo, TEXT("Total Number of ") + FString::FromInt(AssetsFoundList.Num()) + TEXT(" Found\nWould You like to continue?"), false);
	if (ConfirmResult == EAppReturnType::No)
	{
		return;
	}

	TArray<FAssetData> UnusedAssetsData;

	for (const auto& AssetPathName : AssetsFoundList)
	{
		// Skip the root folders
		if (AssetPathName.Contains(TEXT("Collections")) || AssetsFoundList.Contains("Developers"))
		{
			continue;
		}

		if (!UEditorAssetLibrary::DoesAssetExist(AssetPathName))
		{
			continue;
		}

		const TArray<FString> AssetReferencers = UEditorAssetLibrary::FindPackageReferencersForAsset(AssetPathName);

		if (AssetReferencers.IsEmpty())
		{
			const FAssetData UnsuedAssetData = UEditorAssetLibrary::FindAssetData(AssetPathName);
			UnusedAssetsData.Add(UnsuedAssetData);
		}
	}

	if (!UnusedAssetsData.IsEmpty())
	{
		ObjectTools::DeleteAssets(UnusedAssetsData);
	}
	else
	{
		DebugHelper::ShowDialogMessage(EAppMsgType::Ok, TEXT("There are no unused assets found."), true);
	}
	
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FExtendEditorManagerModule, ExtendEditorManager)
