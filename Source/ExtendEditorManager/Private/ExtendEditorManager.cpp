// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExtendEditorManager.h"
#include "ContentBrowserModule.h"

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
		FExecuteAction::CreateLambda([](){})
	);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FExtendEditorManagerModule, ExtendEditorManager)
