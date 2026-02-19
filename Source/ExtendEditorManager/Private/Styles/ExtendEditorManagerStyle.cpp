// Fill out your copyright notice in the Description page of Project Settings.


#include "Styles/ExtendEditorManagerStyle.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/StyleColors.h"

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
	if (CreatedSlateStyle.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*CreatedSlateStyle);
		CreatedSlateStyle.Reset();
	}
}

TSharedRef<FSlateStyleSet> FExtendEditorManagerStyle::CreateSlateStyleSet()
{
	TSharedRef<FSlateStyleSet> CustomStyleSet = MakeShareable(new FSlateStyleSet(StyleSetName));

	const FString IconDirectory = IPluginManager::Get().FindPlugin(TEXT("ExtendEditorManager"))->GetBaseDir() / "Resources";
	CustomStyleSet->SetContentRoot(IconDirectory);

	const FVector2D IconSize = FVector2D(16.0f, 16.0f);
	CustomStyleSet->Set("ContentBrowser.DeleteUnusedAssets", new FSlateImageBrush(IconDirectory / "Bin.png", IconSize));
	CustomStyleSet->Set("ContentBrowser.DeleteEmptyFolders", new FSlateImageBrush(IconDirectory / "FolderFlames.png", IconSize));
	CustomStyleSet->Set("ContentBrowser.AdvanceDelete", new FSlateImageBrush(IconDirectory / "AdvanceDelete.png", IconSize));
	CustomStyleSet->Set("LevelEditor.LockSelection", new FSlateImageBrush(IconDirectory / "Lock.png", IconSize));
	CustomStyleSet->Set("LevelEditor.UnlockSelection", new FSlateImageBrush(IconDirectory / "Unlock.png", IconSize));

	const FCheckBoxStyle SelectionLockToggleButtonStyle = FCheckBoxStyle()
	.SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
	.SetPadding(FMargin(10.0f))
	// Unchecked
	.SetUncheckedImage(FSlateImageBrush(FSlateImageBrush(IconDirectory / "Unlock.png", IconSize, FStyleColors::White25)))
	.SetUncheckedHoveredImage(FSlateImageBrush(FSlateImageBrush(IconDirectory / "Unlock.png", IconSize, FStyleColors::AccentBlue)))
	.SetUncheckedPressedImage(FSlateImageBrush(FSlateImageBrush(IconDirectory / "Unlock.png", IconSize, FStyleColors::Foreground)))
	// Checked
	.SetCheckedImage(FSlateImageBrush(FSlateImageBrush(IconDirectory / "Unlock.png", IconSize, FStyleColors::Foreground)))
	.SetCheckedHoveredImage(FSlateImageBrush(FSlateImageBrush(IconDirectory / "Unlock.png", IconSize, FStyleColors::AccentBlack)))
	.SetCheckedPressedImage(FSlateImageBrush(FSlateImageBrush(IconDirectory / "Unlock.png", IconSize, FStyleColors::AccentGray)));

	CustomStyleSet->Set("SceneOutliner.SelectionLock", SelectionLockToggleButtonStyle);
	
	return CustomStyleSet;
}
