// Copyright (C) 2025 R.Haris Nauman. All Rights Reserved.


#include "Outliner/OutlinerSelectionColumn.h"

#include "ActorTreeItem.h"
#include "ISceneOutlinerTreeItem.h"
#include "Styles/ExtendEditorManagerStyle.h"
#include "ExtendEditorManager.h"

FOutlinerSelectionLockColumn::FOutlinerSelectionLockColumn(ISceneOutliner& SceneOutliner)
{
}

FName FOutlinerSelectionLockColumn::GetColumnID()
{
	return FName("SelectionLock");
}

SHeaderRow::FColumn::FArguments FOutlinerSelectionLockColumn::ConstructHeaderRowColumn()
{
	SHeaderRow::FColumn::FArguments HeaderRow = SHeaderRow::Column(GetColumnID())
	                                            .FixedWidth(24.0f)
	                                            .HAlignHeader(HAlign_Center)
	                                            .VAlignHeader(VAlign_Center)
	                                            .HAlignCell(HAlign_Center)
	                                            .VAlignCell(VAlign_Center)
	                                            .DefaultTooltip(FText::FromString(TEXT("Click to lock selection")))
	[
		SNew(SImage)
		.ColorAndOpacity(FSlateColor::UseForeground())
		.Image(FExtendEditorManagerStyle::GetCreatedStyleSet()->GetBrush("LevelEditor.LockSelection"))
	];

	return HeaderRow;
}

const TSharedRef<SWidget> FOutlinerSelectionLockColumn::ConstructRowWidget(FSceneOutlinerTreeItemRef TreeItem, const STableRow<FSceneOutlinerTreeItemPtr>& Row)
{
	auto* ActorTreeItem = TreeItem->CastTo<FActorTreeItem>();

	if (!ActorTreeItem || !ActorTreeItem->IsValid()) return SNullWidget::NullWidget;

	// Load Extend Editor module
	FExtendEditorManagerModule& ExtendEditorModule = FModuleManager::LoadModuleChecked<FExtendEditorManagerModule>(TEXT("ExtendEditorManager"));

	const bool bIsActorSelectionLocked = ExtendEditorModule.CheckIsActorSelectionLocked(ActorTreeItem->Actor.Get());

	const auto& ToggleButtonStyle = FExtendEditorManagerStyle::GetCreatedStyleSet()->GetWidgetStyle<FCheckBoxStyle>(FName(TEXT("SceneOutliner.SelectionLock")));
	
	TSharedRef<SCheckBox> ConstructedRowWidget = SNew(SCheckBox)
		.Visibility(EVisibility::Visible)
		.Type(ESlateCheckBoxType::ToggleButton)
		.Style(&ToggleButtonStyle)
		.HAlign(HAlign_Center)
		.IsChecked(bIsActorSelectionLocked ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
		.OnCheckStateChanged(this, &FOutlinerSelectionLockColumn::OnRowWidgetCheckStateChanged, ActorTreeItem->Actor);

	return ConstructedRowWidget;
}

void FOutlinerSelectionLockColumn::OnRowWidgetCheckStateChanged(ECheckBoxState NewState, TWeakObjectPtr<AActor> CorrespondingActor)
{
	// Load Extend Editor module
	FExtendEditorManagerModule& ExtendEditorModule = FModuleManager::LoadModuleChecked<FExtendEditorManagerModule>(TEXT("ExtendEditorManager"));
	
	switch (NewState)
	{
		case ECheckBoxState::Checked:
		ExtendEditorModule.ProcessLockingForOutliner(CorrespondingActor.Get(), true);
		break;
		case ECheckBoxState::Unchecked:
		ExtendEditorModule.ProcessLockingForOutliner(CorrespondingActor.Get(), false);
		break;
		case ECheckBoxState::Undetermined:
		break;
		default:
		break;
	}
}
