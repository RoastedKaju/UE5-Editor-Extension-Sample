// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExtendEditorManager.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "DebugHeader.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "SlateWidgets/AdvancedDeletionWidget.h"

#define LOCTEXT_NAMESPACE "FExtendEditorManagerModule"

void FExtendEditorManagerModule::StartupModule()
{
	InitContentBrowserExtension();
	RegisterAdvancedDeletionEditorTab();
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

	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Delete Empty Folders")),
		FText::FromString(TEXT("Tooltip for empty folders")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FExtendEditorManagerModule::DeleteEmptyFolders)
	);

	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Delete Unused Assets And Empty Folders")),
		FText::FromString(TEXT("Tooltip for unused asset deletion")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FExtendEditorManagerModule::DeleteUnusedAssetsAndFolders)
	);

	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Advanced Deletion")),
		FText::FromString(TEXT("Nothing")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FExtendEditorManagerModule::AdvanceDelete)
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

	const EAppReturnType::Type ConfirmResult = DebugHelper::ShowDialogMessage(EAppMsgType::YesNo, TEXT("Total Number of ") + FString::FromInt(AssetsFoundList.Num()) + TEXT(" Assets Found\nWould You like to continue?"), false);
	if (ConfirmResult == EAppReturnType::No)
	{
		return;
	}

	// Redirector fix before discovering unused assets
	FixUpRedirectors();

	TArray<FAssetData> UnusedAssetsData;

	for (const auto& AssetPathName : AssetsFoundList)
	{
		// Skip the root folders
		if (AssetPathName.Contains(TEXT("Collections"))
			|| AssetsFoundList.Contains("Developers")
			|| AssetsFoundList.Contains("__ExternalActors__")
			|| AssetsFoundList.Contains("__ExternalObjects__"))
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

void FExtendEditorManagerModule::FixUpRedirectors()
{
	TArray<UObjectRedirector*> RedirectorsToFix;

	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	FARFilter Filter;
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Emplace("/Game");
	Filter.ClassPaths.Emplace(FTopLevelAssetPath(UObjectRedirector::StaticClass()->GetPathName()));
	TArray<FAssetData> OutRedirectorsData;

	AssetRegistryModule.Get().GetAssets(Filter, OutRedirectorsData);

	for (const auto& RedirectorData : OutRedirectorsData)
	{
		if (UObjectRedirector* RedirectorToFix = Cast<UObjectRedirector>(RedirectorData.GetAsset()))
		{
			RedirectorsToFix.Add(RedirectorToFix);
		}
	}

	const FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
	AssetToolsModule.Get().FixupReferencers(RedirectorsToFix);
}

void FExtendEditorManagerModule::DeleteEmptyFolders()
{
	if (FolderPaths.Num() > 1)
	{
		DebugHelper::ShowDialogMessage(EAppMsgType::Ok, TEXT("You can only do this to one Folder."), true);
		return;
	}

	FixUpRedirectors();

	const TArray<FString> Entries = UEditorAssetLibrary::ListAssets(FolderPaths[0], true, true);

	TArray<FString> DirectoriesToDelete;
	FString EmptyFolderNames;
	for (const auto& Entry : Entries)
	{
		if (UEditorAssetLibrary::DoesDirectoryExist(Entry))
		{
			// Skip the root folders
			if (Entry.Contains(TEXT("Collections"))
				|| Entry.Contains("Developers")
				|| Entry.Contains("__ExternalActors__")
				|| Entry.Contains("__ExternalObjects__"))
			{
				continue;
			}

			// It is important to not have the last '/' when you want to list out the directories
			FString DirectoryPath = Entry;
			DirectoryPath.RemoveFromEnd("/");
			if (!UEditorAssetLibrary::DoesDirectoryHaveAssets(DirectoryPath, false))
			{
				DirectoriesToDelete.Add(Entry);

				EmptyFolderNames.Append(TEXT("\n") + DirectoryPath);
			}
		}
	}

	if (DirectoriesToDelete.IsEmpty())
	{
		DebugHelper::ShowNotification(TEXT("No Empty Directories Found."));
		return;
	}

	const EAppReturnType::Type DialogResult = DebugHelper::ShowDialogMessage(EAppMsgType::OkCancel, TEXT("Are You Sure You Want to Delete The Follow Folders?") + EmptyFolderNames, false);

	if (DialogResult == EAppReturnType::Ok)
	{
		for (const auto& Directory : DirectoriesToDelete)
		{
			UEditorAssetLibrary::DeleteDirectory(Directory);
		}
	}
}

void FExtendEditorManagerModule::DeleteUnusedAssetsAndFolders()
{
	DeleteUnusedAssets();
	DeleteEmptyFolders();
}

void FExtendEditorManagerModule::AdvanceDelete()
{
	FGlobalTabmanager::Get()->TryInvokeTab(FName("AdvancedDeletion"));
}

void FExtendEditorManagerModule::RegisterAdvancedDeletionEditorTab()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(FName("AdvancedDeletion"), FOnSpawnTab::CreateRaw(this, &FExtendEditorManagerModule::OnSpawnAdvancedDeletionEditorTab))
	.SetDisplayName(FText::FromString("Advanced Deletion"));
}

TSharedRef<SDockTab> FExtendEditorManagerModule::OnSpawnAdvancedDeletionEditorTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab).TabRole(NomadTab)
		[
			SNew(SAdvancedDeletionWidget)
			.TestString("I Am Passing Some Data")
		];
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FExtendEditorManagerModule, ExtendEditorManager)
