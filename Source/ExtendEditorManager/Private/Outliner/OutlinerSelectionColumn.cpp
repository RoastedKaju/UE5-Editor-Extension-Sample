// Copyright (C) 2025 R.Haris Nauman. All Rights Reserved.


#include "Outliner/OutlinerSelectionColumn.h"
#include "Styles/ExtendEditorManagerStyle.h"

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
	TSharedRef<SCheckBox> ConstructedRowWidget = SNew(SCheckBox)
	.Visibility(EVisibility::Visible)
	.HAlign(HAlign_Center)
	.IsChecked(ECheckBoxState::Unchecked);

	return ConstructedRowWidget;
}
