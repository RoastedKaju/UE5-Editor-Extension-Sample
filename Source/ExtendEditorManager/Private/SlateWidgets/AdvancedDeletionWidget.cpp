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

	FSlateFontInfo ClassNameFontStyle = FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
	ClassNameFontStyle.Size = 10.0f;

	FSlateFontInfo AssetNameFontStyle = FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
	AssetNameFontStyle.Size = 15.0f;

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
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Fill)
			.FillWidth(0.35f)
			[
				ConstructTextBlock(AssetDataToDisplay->AssetClassPath.GetAssetName().ToString(), ClassNameFontStyle)
			]
			// Asset name slot
			+ SHorizontalBox::Slot()
			[
				ConstructTextBlock(AssetDataToDisplay->AssetName.ToString(), AssetNameFontStyle)
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

TSharedRef<STextBlock> SAdvancedDeletionWidget::ConstructTextBlock(const FString& TextToDisplay, const FSlateFontInfo& Font)
{
	TSharedRef<STextBlock> TextBlock = SNew(STextBlock)
		.Text(FText::FromString(TextToDisplay))
		.Font(Font)
		.ColorAndOpacity(FColor::White);

	return TextBlock;
}
