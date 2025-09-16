// Fill out your copyright notice in the Description page of Project Settings.


#include "SlateWidgets/AdvancedDeletionWidget.h"
#include "DebugHeader.h"
#include "ExtendEditorManager.h"

void SAdvancedDeletionWidget::Construct(const FArguments& args)
{
	bCanSupportFocus = true;

	AssetsData = args._AssetDataArray;

	FSlateFontInfo TitleFontInfo = FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
	TitleFontInfo.Size = 30;

	FSlateFontInfo ButtonFont = FCoreStyle::Get().GetFontStyle(FName("NormalBold"));
	ButtonFont.Size = 24.0f;

	ChildSlot
	[
		SNew(SVerticalBox)
		// Title slot
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Advanced Deletion"))
			.Font(TitleFontInfo)
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
			ConstructListView()
		]
		// Buttons slot
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.Padding(5)
			[
				SNew(SButton)
				.OnClicked(this, &SAdvancedDeletionWidget::OnDeleteAllButtonClicked)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Delete All"))
					.Justification(ETextJustify::Center)
					.Font(ButtonFont)
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.Padding(5)
			[
				SNew(SButton)
				.OnClicked(this, &SAdvancedDeletionWidget::OnSelectAllButtonClicked)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Select All"))
					.Justification(ETextJustify::Center)
					.Font(ButtonFont)
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.Padding(5)
			[
				SNew(SButton)
				.OnClicked(this, &SAdvancedDeletionWidget::OnDeselectAllButtonClicked)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Deselect All"))
					.Justification(ETextJustify::Center)
					.Font(ButtonFont)
				]
			]
		]
	];
}

TSharedRef<SListView<TSharedPtr<FAssetData>>> SAdvancedDeletionWidget::ConstructListView()
{
	ListViewPtr = SNew(SListView<TSharedPtr<FAssetData>>)
		.ListItemsSource(&AssetsData)
		.OnGenerateRow(this, &SAdvancedDeletionWidget::OnGenerateRowListView);

	return ListViewPtr.ToSharedRef();
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
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Left)
			[
				ConstructTextBlock(AssetDataToDisplay->AssetName.ToString(), AssetNameFontStyle)
			]
			// Button slot
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Right)
			[
				ConstructDeleteButton(AssetDataToDisplay)
			]
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

TSharedRef<SButton> SAdvancedDeletionWidget::ConstructDeleteButton(TSharedPtr<FAssetData> AssetData)
{
	TSharedRef<SButton> DeleteButton = SNew(SButton)
		.Text(FText::FromString("Delete"))
		.OnClicked(this, &SAdvancedDeletionWidget::OnDeleteButtonClicked, AssetData);

	return DeleteButton;
}

FReply SAdvancedDeletionWidget::OnDeleteButtonClicked(TSharedPtr<FAssetData> AssetData)
{
	FExtendEditorManagerModule& ExtendEditorManager = FModuleManager::LoadModuleChecked<FExtendEditorManagerModule>(TEXT("ExtendEditorManager"));

	const bool bIsAssetDeleted = ExtendEditorManager.RequestDeleteAsset(*AssetData.Get());
	if (bIsAssetDeleted)
	{
		if (AssetsData.Contains(AssetData))
		{
			AssetsData.Remove(AssetData);
		}

		if (ListViewPtr.IsValid())
		{
			ListViewPtr->RebuildList();
		}
	}

	return FReply::Handled();
}

FReply SAdvancedDeletionWidget::OnDeleteAllButtonClicked()
{
	DebugHelper::ShowNotification(TEXT("Delete All Assets"));
	return FReply::Handled();
}

FReply SAdvancedDeletionWidget::OnSelectAllButtonClicked()
{
	DebugHelper::ShowNotification(TEXT("Select All Assets"));
	return FReply::Handled();
}

FReply SAdvancedDeletionWidget::OnDeselectAllButtonClicked()
{
	DebugHelper::ShowNotification(TEXT("Deselect All Assets"));
	return FReply::Handled();
}
