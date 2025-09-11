// Fill out your copyright notice in the Description page of Project Settings.


#include "SlateWidgets/AdvancedDeletionWidget.h"
#include "DebugHeader.h"

void SAdvancedDeletionWidget::Construct(const FArguments& args)
{
	bCanSupportFocus = true;

	AssetsData = args._AssetDataArray;

	FSlateFontInfo FontInfo = FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
	FontInfo.Size = 30;

	ChildSlot
	[
		SNew(SVerticalBox)
		// Title slot
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Advanced Deletion"))
			.Font(FontInfo)
			.Justification(ETextJustify::Center)
			.ColorAndOpacity(FColor::White)
		]
		// Conditions slot
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Conditions Go Here."))
		]
		// Asset List slot
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		[
			SNew(SListView<TSharedPtr<FAssetData>>)
			.ListItemsSource(&AssetsData)
			.OnGenerateRow(this, &SAdvancedDeletionWidget::OnGenerateRowListView)
		]
		// Buttons slot
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
		]
	];
}

TSharedRef<ITableRow> SAdvancedDeletionWidget::OnGenerateRowListView(TSharedPtr<FAssetData> AssetDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable)
{
	if (!AssetDataToDisplay.IsValid())
	{
		return SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable);
	}

	auto GeneratedRow = SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable)
		[
			SNew(SHorizontalBox)
			// Checkbox slot
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			.FillWidth(0.05f)
			[
				ConstructCheckBox(AssetDataToDisplay)
			]
			// Asset class name slot

			// Asset name slot
			+ SHorizontalBox::Slot()
			[
				SNew(STextBlock)
				.Text(FText::FromName(AssetDataToDisplay->AssetName))
			]

			// Button slot
		];

	return GeneratedRow;
}

TSharedRef<SCheckBox> SAdvancedDeletionWidget::ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	auto ConstructedCheckBox = SNew(SCheckBox)
		.Type(ESlateCheckBoxType::CheckBox)
		.OnCheckStateChanged(this, &SAdvancedDeletionWidget::OnCheckBoxStateChanged, AssetDataToDisplay)
		.Visibility(EVisibility::Visible);

	return ConstructedCheckBox;
}

void SAdvancedDeletionWidget::OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData)
{
	switch (NewState)
	{
	case ECheckBoxState::Checked:
		DebugHelper::ShowNotification(AssetData->AssetName.ToString() + TEXT(" Checked"));
		break;
	case ECheckBoxState::Unchecked:
		DebugHelper::ShowNotification(AssetData->AssetName.ToString() + TEXT(" Unchecked"));
		break;
	case ECheckBoxState::Undetermined:
		break;
	default:
		break;
	}
}
