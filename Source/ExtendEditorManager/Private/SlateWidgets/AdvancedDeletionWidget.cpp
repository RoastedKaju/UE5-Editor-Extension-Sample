// Fill out your copyright notice in the Description page of Project Settings.


#include "SlateWidgets/AdvancedDeletionWidget.h"

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
		// Listing condition slot
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SListView<TSharedPtr<FAssetData>>)
			.ListItemsSource(&AssetsData)
			.OnGenerateRow(this, &SAdvancedDeletionWidget::OnGenerateRowListView)
		]
		// Asset List slot
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SScrollBox)	
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
	auto GeneratedRow = SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable)
		[
			SNew(STextBlock)
			.Text(FText::FromName(AssetDataToDisplay->AssetName))
		];

	return GeneratedRow;
}
