// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomToolsEditorModule.h"
#include "ToolMenus.h"
#include "ToolMenu.h"

#define LOCTEXT_NAMESPACE "CustomToolsEditorModule"

IMPLEMENT_GAME_MODULE(FCustomToolsEditorModule, CustomToolsEditorModule);

DEFINE_LOG_CATEGORY(CustomToolsEditorLog);

void FCustomToolsEditorModule::StartupModule()
{
	UE_LOG(CustomToolsEditorLog, Log, TEXT("Starting up Editor Extensions Editor module."));

	// Register tool menus callback
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FCustomToolsEditorModule::RegisterMenus));
}

void FCustomToolsEditorModule::ShutdownModule()
{
}

void FCustomToolsEditorModule::RegisterMenus()
{
	UE_LOG(CustomToolsEditorLog, Log, TEXT("Tool Menus ready for use."));

	// Extend the level editor main toolbar
	UToolMenu* ToolBarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");

	// Section
	FToolMenuSection& Section = ToolBarMenu->AddSection(TEXT("EditorExtensions_WizardSection"), LOCTEXT("SectionLabel", "Wizard Section"));

	// Button entry
	{
		const FToolMenuEntry Entry = FToolMenuEntry::InitToolBarButton
		(
			TEXT("WizardButton"),
			FUIAction(FExecuteAction::CreateRaw(this, &FCustomToolsEditorModule::OnWizardButtonClicked)),
			LOCTEXT("WizardButton_Label", "Wizard Widget"),
			LOCTEXT("WizardButton_ToolTip", "Opens up the wizard widget."),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.GameSettings")
		);

		Section.AddEntry(Entry);
	}
	// Another entry with default icon and text showing
	{
		FToolMenuEntry Entry = FToolMenuEntry::InitToolBarButton
		(
			TEXT("DefaultWizardButton"),
			FUIAction(FExecuteAction::CreateRaw(this, &FCustomToolsEditorModule::OnWizardButtonClicked)),
			LOCTEXT("WizardButton_Label", "Wizard Text"),
			LOCTEXT("WizardButton_ToolTip", "Opens up the wizard widget."),
			FSlateIcon()
		);

		// Use this to show label alongside the button icon
		Entry.StyleNameOverride = "CalloutToolBar";
		Section.AddEntry(Entry);
	}
	// Combo button entry
	{
		FNewToolMenuChoice WizardComboMenuChoice = FNewMenuDelegate::CreateLambda([](FMenuBuilder& MenuBuilder)
		{
			// Entry item 1
			MenuBuilder.AddMenuEntry(
				LOCTEXT("WizardLabel_Item1", "Item 1"),
				LOCTEXT("WizardLabel_ToolTip", "Item 1 Tool Tip"),
				FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.GameSettings"), // Empty if you want no Icon
				FUIAction()
			);

			// Entry item 2
			MenuBuilder.AddMenuEntry(
				LOCTEXT("WizardLabel_Item2", "Item 2"),
				LOCTEXT("WizardLabel_ToolTip", "Item 2 Tool Tip"),
				FSlateIcon(FAppStyle::GetAppStyleSetName(), "MainFrame.PackageProject"),
				FUIAction()
			);
		});

		FToolMenuEntry Entry = FToolMenuEntry::InitComboButton
		(
			TEXT("WizardComboButton"),
			FUIAction(),
			WizardComboMenuChoice,
			LOCTEXT("WizardCombo_Label", "Wizard"),
			LOCTEXT("WizardCombo_Tooltip", "Opens up the wizard widget."),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "MainFrame.PackageProject"),
			true //bInSimpleComboBox
		);

		Entry.StyleNameOverride = "CalloutToolBar";
		Section.AddEntry(Entry);
	}
}

void FCustomToolsEditorModule::OnWizardButtonClicked()
{
	UE_LOG(CustomToolsEditorLog, Log, TEXT("Opening editor wizard example widget."));
}

#undef LOCTEXT_NAMESPACE
