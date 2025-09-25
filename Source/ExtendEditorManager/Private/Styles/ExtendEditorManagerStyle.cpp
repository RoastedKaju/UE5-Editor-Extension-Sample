// Fill out your copyright notice in the Description page of Project Settings.


#include "Styles/ExtendEditorManagerStyle.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

FName FExtendEditorManagerStyle::StyleSetName = FName("ExtendEditorManagerStyle");
TSharedPtr<FSlateStyleSet> FExtendEditorManagerStyle::CreatedSlateStyle = nullptr;

void FExtendEditorManagerStyle::InitializeIcons()
{
	if (!CreatedSlateStyle.IsValid())
	{
		CreatedSlateStyle = CreateSlateStyleSet();
		FSlateStyleRegistry::RegisterSlateStyle(*CreatedSlateStyle);
	}
}

void FExtendEditorManagerStyle::Shutdown()
{
}

TSharedRef<FSlateStyleSet> FExtendEditorManagerStyle::CreateSlateStyleSet()
{
	TSharedRef<FSlateStyleSet> CustomStyleSet = MakeShareable(new FSlateStyleSet(StyleSetName));

	const FString IconDirectory = IPluginManager::Get().FindPlugin(TEXT("ExtendEditorManager"))->GetBaseDir() / "Resources";
	CustomStyleSet->SetContentRoot(IconDirectory);
	
	const FVector2D IconSize = FVector2D(16.0f, 16.0f);
	CustomStyleSet->Set("ContentBrowser.DeleteUnusedAssets", new FSlateImageBrush(IconDirectory/"Bin.png", IconSize));
	
	return CustomStyleSet;
}
