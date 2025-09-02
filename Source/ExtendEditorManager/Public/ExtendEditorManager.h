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
	void InitContentBrowserExtension();
	TSharedRef<FExtender> CustomCBMenuExtender(const TArray<FString>& SelectedPaths);
	void AddContentBrowserMenuEntry(FMenuBuilder& MenuBuilder);
	void DeleteUnusedAssets();
};
