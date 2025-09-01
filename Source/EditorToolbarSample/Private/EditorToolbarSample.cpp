#include "EditorToolbarSample.h"
#include "ToolMenus.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Widgets/SimpleCompoundWidget.h"

#define LOCTEXT_NAMESPACE "FEditorToolbarSampleModule"

DEFINE_LOG_CATEGORY(EditorToolbarSample)

void FEditorToolbarSampleModule::StartupModule()
{
	UE_LOG(EditorToolbarSample, Display, TEXT("Startup Module Editor Toolbar Sample."));

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FEditorToolbarSampleModule::RegisterToolbarExtension));
}

void FEditorToolbarSampleModule::ShutdownModule()
{
}

void FEditorToolbarSampleModule::RegisterToolbarExtension()
{
	UE_LOG(EditorToolbarSample, Display, TEXT("Toolbar Extension Registered"));

	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
	FToolMenuSection& Section = ToolbarMenu->AddSection("EditorToolbarSampleWizard", LOCTEXT("SectionLabel", "Wizard Section"));

	// Button without text
	{
		FToolMenuEntry Entry = FToolMenuEntry::InitToolBarButton
		(
			TEXT("WizardButton"),
			FUIAction(),
			LOCTEXT("WizardButton", "Wizard Button"),
			LOCTEXT("ToolbarButton", "Toolbar Button"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.GameSettings")
		);

		Section.AddEntry(Entry);
	}
	// Button with text
	{
		FToolMenuEntry Entry = FToolMenuEntry::InitToolBarButton
		(
			TEXT("WizardButtonWithText"),
			FUIAction(),
			LOCTEXT("WizardButtonWithText", "Wizard Button"),
			LOCTEXT("ToolbarButtonWithText", "Toolbar Button"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.GameSettings")
		);

		Entry.StyleNameOverride = "CalloutToolBar";
		Section.AddEntry(Entry);
	}
	// Combo Button With Text
	{
		FToolMenuEntry Entry = FToolMenuEntry::InitComboButton
		(
			TEXT("WizardComboWithText"),
			FUIAction(),
			CreateComboChoices(),
			LOCTEXT("WizardComboWithText", "Wizard Combo"),
			LOCTEXT("ToolbarComboWithText", "Toolbar Combo"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.GameSettings"),
			true
		);

		Entry.StyleNameOverride = "CalloutToolBar";
		Section.AddEntry(Entry);
	}
}

FNewToolMenuChoice FEditorToolbarSampleModule::CreateComboChoices()
{
	FNewToolMenuChoice Choice = FNewMenuDelegate::CreateLambda([&](FMenuBuilder& MenuBuilder)
	{
		MenuBuilder.AddMenuEntry
		(
			LOCTEXT("LabelA","Show Notification"),
			LOCTEXT("LabelA-ToolTip","Notification tooltip"),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateRaw(this, &FEditorToolbarSampleModule::ShowNotification))
		);
		MenuBuilder.AddMenuEntry
		(
			LOCTEXT("LabelB", "Show Compound Widget"),
			LOCTEXT("LabelB-ToolTip", "Compound Widget tooltip"),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateRaw(this, &FEditorToolbarSampleModule::ShowCompoundWidget))
		);
		MenuBuilder.AddMenuEntry
		(
			LOCTEXT("LabelC", "Show Dialog Box"),
			LOCTEXT("LabelC-ToolTip", "Dialog Box tooltip"),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateRaw(this, &FEditorToolbarSampleModule::ShowDialogBox))
		);
	});
	return Choice;
}

void FEditorToolbarSampleModule::ShowNotification()
{
	FNotificationInfo Notification(FText::FromString("Notification Button Pressed!"));
	Notification.bUseLargeFont = true;
	Notification.ExpireDuration = 5.0f;

	FSlateNotificationManager::Get().AddNotification(Notification);
}

void FEditorToolbarSampleModule::ShowDialogBox()
{
	FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("Dialog Box OK", "OK"), LOCTEXT("Dialog Box Title", "Title"));
}

void FEditorToolbarSampleModule::ShowCompoundWidget()
{
	const TSharedRef<SWindow> Window = SNew(SWindow)
		.Title(LOCTEXT("Window Title", "Simple Compound Widget"))
		.ClientSize(FVector2D(1200, 200))
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		.SizingRule(ESizingRule::UserSized);

	Window->SetContent(SNew(SimpleCompoundWidget));

	FSlateApplication::Get().AddModalWindow(Window, FSlateApplication::Get().GetActiveTopLevelWindow());
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FEditorToolbarSampleModule, EditorToolbarSample)
