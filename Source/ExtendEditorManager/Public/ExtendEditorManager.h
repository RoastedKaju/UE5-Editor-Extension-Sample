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
};
