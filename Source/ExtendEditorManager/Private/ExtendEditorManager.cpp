// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExtendEditorManager.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "DebugHeader.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "SlateWidgets/AdvancedDeletionWidget.h"
#include "Styles/ExtendEditorManagerStyle.h"
#include "LevelEditor.h"
#include "Engine/Selection.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "Commands/ExtendEditorUICommands.h"
#include "SceneOutlinerModule.h"
#include "Outliner/OutlinerSelectionColumn.h"
#include "SSceneOutliner.h"

#define LOCTEXT_NAMESPACE "FExtendEditorManagerModule"

void FExtendEditorManagerModule::StartupModule()
{
	InitContentBrowserExtension();
	RegisterAdvancedDeletionEditorTab();

	FExtendEditorManagerStyle::InitializeIcons();

	FExtendEditorUICommands::Register();
	InitCustomUICommands();

	InitLevelActorMenuExtension();

	InitCustomSelectionEvent();

	InitSceneOutlinerExtension();
}

void FExtendEditorManagerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(FName("AdvancedDeletion"));

	FExtendEditorManagerStyle::Shutdown();
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
		FSlateIcon(FExtendEditorManagerStyle::GetStyleName(), "ContentBrowser.DeleteUnusedAssets"),
		FExecuteAction::CreateRaw(this, &FExtendEditorManagerModule::DeleteUnusedAssets)
	);

	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Delete Empty Folders")),
		FText::FromString(TEXT("Tooltip for empty folders")),
		FSlateIcon(FExtendEditorManagerStyle::GetStyleName(), "ContentBrowser.DeleteEmptyFolders"),
		FExecuteAction::CreateRaw(this, &FExtendEditorManagerModule::DeleteEmptyFolders)
	);

	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Delete Unused Assets And Empty Folders")),
		FText::FromString(TEXT("Tooltip for unused asset deletion")),
		FSlateIcon(FExtendEditorManagerStyle::GetStyleName(), "ContentBrowser.DeleteEmptyFolders"),
		FExecuteAction::CreateRaw(this, &FExtendEditorManagerModule::DeleteUnusedAssetsAndFolders)
	);

	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Advanced Deletion")),
		FText::FromString(TEXT("Nothing")),
		FSlateIcon(FExtendEditorManagerStyle::GetStyleName(), "ContentBrowser.AdvanceDelete"),
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
	FixUpRedirectors();

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
			.AssetDataArray(GetAllAssetDataInSelectedFolder())
			.FolderPath(FolderPaths[0])
		];
}

TArray<TSharedPtr<FAssetData>> FExtendEditorManagerModule::GetAllAssetDataInSelectedFolder()
{
	if (FolderPaths.IsEmpty())
	{
		return TArray<TSharedPtr<FAssetData>>();
	}

	TArray<TSharedPtr<FAssetData>> AvailableAssetsData;

	TArray<FString> AssetsFoundList = UEditorAssetLibrary::ListAssets(FolderPaths[0]);
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

		UE_LOG(LogTemp, Log, TEXT("Found Asset: %s"), *AssetPathName);

		FAssetData AssetData = UEditorAssetLibrary::FindAssetData(AssetPathName);
		AvailableAssetsData.Add(MakeShared<FAssetData>(AssetData));
	}

	return AvailableAssetsData;
}

void FExtendEditorManagerModule::InitLevelActorMenuExtension()
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	// Hot keys
	TSharedRef<FUICommandList> ExistingLevelCommands = LevelEditorModule.GetGlobalLevelEditorActions();
	ExistingLevelCommands->Append(CustomUICommands.ToSharedRef());

	auto& LevelEditorMenuExtenders = LevelEditorModule.GetAllLevelViewportContextMenuExtenders();

	LevelEditorMenuExtenders.Add(FLevelEditorModule::FLevelViewportMenuExtender_SelectedActors::CreateRaw(this, &FExtendEditorManagerModule::CustomLevelEditorMenuExtender));
}

TSharedRef<FExtender> FExtendEditorManagerModule::CustomLevelEditorMenuExtender(const TSharedRef<FUICommandList> UICommandList, const TArray<AActor*> SelectedActors)
{
	TSharedRef<FExtender> MenuExtender = MakeShareable(new FExtender());

	if (!SelectedActors.IsEmpty())
	{
		MenuExtender->AddMenuExtension(
			FName("ActorOptions"),
			EExtensionHook::Before,
			UICommandList,
			FMenuExtensionDelegate::CreateRaw(this, &FExtendEditorManagerModule::AddLevelEditorMenuEntry));
	}

	return MenuExtender;
}

void FExtendEditorManagerModule::AddLevelEditorMenuEntry(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		FText::FromString("Lock Actor Selection"),
		FText::FromString("Prevent Actor from being selected"),
		FSlateIcon(FExtendEditorManagerStyle::GetStyleName(), "LevelEditor.LockSelection"),
		FExecuteAction::CreateRaw(this, &FExtendEditorManagerModule::OnLockActorSelectionButtonClicked));

	MenuBuilder.AddMenuEntry(
		FText::FromString("Unlock All Actors Selection"),
		FText::FromString("Prevent Actor from being selected"),
		FSlateIcon(FExtendEditorManagerStyle::GetStyleName(), "LevelEditor.UnlockSelection"),
		FExecuteAction::CreateRaw(this, &FExtendEditorManagerModule::OnUnlockAllActorsSelectionButtonClicked));
}

void FExtendEditorManagerModule::OnLockActorSelectionButtonClicked()
{
	if (!GetEditorActorSubsystem()) return;

	TArray<AActor*> SelectedActors = WeakEditorActorSubsystem->GetSelectedLevelActors();
	if (SelectedActors.IsEmpty())
		return;

	for (AActor* SelectedActor : SelectedActors)
	{
		if (!IsValid(SelectedActor))
			continue;

		LockActorSelection(SelectedActor);
		WeakEditorActorSubsystem->SetActorSelectionState(SelectedActor, false);
	}

	RefreshSceneOutliner();
}

void FExtendEditorManagerModule::OnUnlockAllActorsSelectionButtonClicked()
{
	if (!GetEditorActorSubsystem()) return;

	TArray<AActor*> AllLevelActors = WeakEditorActorSubsystem->GetAllLevelActors();
	if (AllLevelActors.IsEmpty()) return;

	for (AActor* Actor : AllLevelActors)
	{
		if (!IsValid(Actor)) continue;

		if (CheckIsActorSelectionLocked(Actor))
		{
			UnlockActorSelection(Actor);
		}
	}

	RefreshSceneOutliner();
}

void FExtendEditorManagerModule::InitCustomSelectionEvent()
{
	USelection* UserSelection = GEditor->GetSelectedActors();

	UserSelection->SelectObjectEvent.AddRaw(this, &FExtendEditorManagerModule::OnActorSelected);
}

void FExtendEditorManagerModule::OnActorSelected(UObject* SelectedObject)
{
	if (!GetEditorActorSubsystem())
	{
		return;
	}

	if (AActor* SelectedActor = Cast<AActor>(SelectedObject))
	{
		if (CheckIsActorSelectionLocked(SelectedActor))
		{
			WeakEditorActorSubsystem->SetActorSelectionState(SelectedActor, false);
		}
	}
}

void FExtendEditorManagerModule::LockActorSelection(AActor* ActorToProcess)
{
	if (ActorToProcess == nullptr) return;

	if (!ActorToProcess->ActorHasTag(FName("Locked")))
	{
		ActorToProcess->Tags.Add(FName("Locked"));
	}
}

void FExtendEditorManagerModule::UnlockActorSelection(AActor* ActorToProcess)
{
	if (ActorToProcess == nullptr) return;

	if (ActorToProcess->ActorHasTag(FName("Locked")))
	{
		ActorToProcess->Tags.Remove(FName("Locked"));
	}
}

bool FExtendEditorManagerModule::CheckIsActorSelectionLocked(AActor* ActorToProcess)
{
	if (ActorToProcess == nullptr)
		return false;

	return ActorToProcess->ActorHasTag(FName("Locked"));
}

void FExtendEditorManagerModule::ProcessLockingForOutliner(AActor* ActorToProcess, bool LockState)
{
	if (!GetEditorActorSubsystem()) return;

	if (LockState)
	{
		LockActorSelection(ActorToProcess);
		WeakEditorActorSubsystem->SetActorSelectionState(ActorToProcess, true);
	}
	else
	{
		UnlockActorSelection(ActorToProcess);
		WeakEditorActorSubsystem->SetActorSelectionState(ActorToProcess, false);
	}
}

bool FExtendEditorManagerModule::GetEditorActorSubsystem()
{
	if (!WeakEditorActorSubsystem.IsValid())
	{
		WeakEditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	}

	return WeakEditorActorSubsystem.IsValid();
}

void FExtendEditorManagerModule::InitCustomUICommands()
{
	CustomUICommands = MakeShareable(new FUICommandList());

	CustomUICommands->MapAction(FExtendEditorUICommands::Get().LockActorSelection, FExecuteAction::CreateRaw(this, &FExtendEditorManagerModule::OnSelectionLockHotkeyPressed));
	CustomUICommands->MapAction(FExtendEditorUICommands::Get().UnlockActorSelection, FExecuteAction::CreateRaw(this, &FExtendEditorManagerModule::OnUnlockAllActorsSelectionButtonClicked));
}

void FExtendEditorManagerModule::OnSelectionLockHotkeyPressed()
{
	OnLockActorSelectionButtonClicked();
}

void FExtendEditorManagerModule::OnSelectionUnlockHotkeyPressed()
{
	OnUnlockAllActorsSelectionButtonClicked();
}

void FExtendEditorManagerModule::InitSceneOutlinerExtension()
{
	FSceneOutlinerModule& SceneOutlinerModule = FModuleManager::LoadModuleChecked<FSceneOutlinerModule>(TEXT("SceneOutliner"));
	FSceneOutlinerColumnInfo SelectionLockColumnInfo(
		ESceneOutlinerColumnVisibility::Visible,
		1,
		FCreateSceneOutlinerColumn::CreateRaw(this, &FExtendEditorManagerModule::OnCreateSceneOutlinerColumn)
	);

	SceneOutlinerModule.RegisterDefaultColumnType<FOutlinerSelectionLockColumn>(SelectionLockColumnInfo);
}

TSharedRef<ISceneOutlinerColumn> FExtendEditorManagerModule::OnCreateSceneOutlinerColumn(ISceneOutliner& SceneOutliner)
{
	return MakeShareable(new FOutlinerSelectionLockColumn(SceneOutliner));
}

void FExtendEditorManagerModule::RefreshSceneOutliner()
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	auto SceneOutliners = LevelEditorModule.GetFirstLevelEditor()->GetAllSceneOutliners();

	for (const TWeakPtr<ISceneOutliner>& WeakOutliner : SceneOutliners)
	{
		TSharedPtr<ISceneOutliner> Outliner = WeakOutliner.Pin();
		if (!Outliner.IsValid())
			continue;

		TSharedPtr<SSceneOutliner> SceneOutliner = StaticCastSharedPtr<SSceneOutliner>(Outliner);

		if (SceneOutliner.IsValid())
		{
			SceneOutliner->FullRefresh();
		}
	}
}

bool FExtendEditorManagerModule::RequestDeleteAsset(const FAssetData& AssetData) const
{
	TArray<FAssetData> AssetToDelete = {AssetData};
	if (ObjectTools::DeleteAssets(AssetToDelete) > 0)
	{
		return true;
	}

	return false;
}

bool FExtendEditorManagerModule::RequestMultipleDeleteAssets(const TArray<FAssetData>& Assets) const
{
	TArray<FAssetData> AssetToDelete = Assets;
	if (ObjectTools::DeleteAssets(AssetToDelete) > 0)
	{
		return true;
	}

	return false;
}

void FExtendEditorManagerModule::ListUnusedAssets(const TArray<TSharedPtr<FAssetData>>& AssetDataToFilter, TArray<TSharedPtr<FAssetData>>& OutUnusedAssets)
{
	OutUnusedAssets.Empty();

	for (const auto& AssetData : AssetDataToFilter)
	{
		const TArray<FString> AssetReferencers = UEditorAssetLibrary::FindPackageReferencersForAsset(AssetData->GetObjectPathString());

		if (AssetReferencers.IsEmpty())
		{
			OutUnusedAssets.Add(AssetData);
		}
	}
}

void FExtendEditorManagerModule::ListSameNameAssets(const TArray<TSharedPtr<FAssetData>>& AssetDataToFilter, TArray<TSharedPtr<FAssetData>>& OutSameNameAssets)
{
	OutSameNameAssets.Empty();
	TMultiMap<FString, TSharedPtr<FAssetData>> SameNameMultiMap;

	for (const auto& AssetData : AssetDataToFilter)
	{
		SameNameMultiMap.Add(AssetData->AssetName.ToString(), AssetData);
	}

	TArray<FString> Keys;
	SameNameMultiMap.GetKeys(Keys);
	for (const auto& Key : Keys)
	{
		if (SameNameMultiMap.Num(Key) > 1)
		{
			TArray<TSharedPtr<FAssetData>> Assets;
			SameNameMultiMap.MultiFind(Key, Assets);

			OutSameNameAssets.Append(Assets);
		}
	}
}

void FExtendEditorManagerModule::GoToAssetInContentBrowser(const FString& AssetPath)
{
	const TArray<FString> AssetPaths{AssetPath};
	UEditorAssetLibrary::SyncBrowserToObjects(AssetPaths);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FExtendEditorManagerModule, ExtendEditorManager)
