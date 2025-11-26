// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FExtendEditorManagerModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

protected:
	TArray<FString> FolderPaths;

private:
	// Content browser extension
	void InitContentBrowserExtension();
	TSharedRef<FExtender> CustomCBMenuExtender(const TArray<FString>& SelectedPaths);
	void AddContentBrowserMenuEntry(FMenuBuilder& MenuBuilder);
	void DeleteUnusedAssets();
	void FixUpRedirectors();
	void DeleteEmptyFolders();
	void DeleteUnusedAssetsAndFolders();
	void AdvanceDelete();

	// Editor Tab
	void RegisterAdvancedDeletionEditorTab();
	TSharedRef<SDockTab> OnSpawnAdvancedDeletionEditorTab(const FSpawnTabArgs& SpawnTabArgs);
	TArray<TSharedPtr<FAssetData>> GetAllAssetDataInSelectedFolder();

	// Level Viewport Context Extenders
	void InitLevelActorMenuExtension();
	TSharedRef<FExtender> CustomLevelEditorMenuExtender(const TSharedRef<FUICommandList> UICommandList, const TArray<AActor*> SelectedActors);
	void AddLevelEditorMenuEntry(FMenuBuilder& MenuBuilder);
	void OnLockActorSelectionButtonClicked();
	void OnUnlockAllActorsSelectionButtonClicked();

	// Actor Selection
	void InitCustomSelectionEvent();
	void OnActorSelected(UObject* SelectedObject);
	
public:
	bool RequestDeleteAsset(const FAssetData& AssetData) const;
	bool RequestMultipleDeleteAssets(const TArray<FAssetData>& Assets) const;
	void ListUnusedAssets(const TArray<TSharedPtr<FAssetData>>& AssetDataToFilter, TArray<TSharedPtr<FAssetData>>& OutUnusedAssets);
	void ListSameNameAssets(const TArray<TSharedPtr<FAssetData>>& AssetDataToFilter, TArray<TSharedPtr<FAssetData>>& OutSameNameAssets);
	void GoToAssetInContentBrowser(const FString& AssetPath);
};
