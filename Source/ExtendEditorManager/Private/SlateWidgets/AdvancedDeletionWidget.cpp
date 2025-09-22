// Fill out your copyright notice in the Description page of Project Settings.


#include "SlateWidgets/AdvancedDeletionWidget.h"
#include "DebugHeader.h"
#include "ExtendEditorManager.h"

void SAdvancedDeletionWidget::Construct(const FArguments& args)
{
	bCanSupportFocus = true;

	AssetsData = args._AssetDataArray;
	DisplayedAssetsData = AssetsData;

	FSlateFontInfo TitleFontInfo = FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
	TitleFontInfo.Size = 30;

	FSlateFontInfo ButtonFont = FCoreStyle::Get().GetFontStyle(FName("NormalBold"));
	ButtonFont.Size = 24.0f;

	CheckBoxes.Empty();
	AssetsToDelete.Empty();

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
		.Padding(15.0f)
		.HAlign(HAlign_Left)
		[
			ConstructComboBox()
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
					.Text(FText::FromString("Delete Selected"))
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
		.ListItemsSource(&DisplayedAssetsData)
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

	// Add the created checkbox into checkboxes list
	CheckBoxes.Add(ConstructedCheckBox);
	
	return ConstructedCheckBox;
}

void SAdvancedDeletionWidget::OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData)
{
	switch (NewState)
	{
	case ECheckBoxState::Checked:
		AssetsToDelete.Add(AssetData);
		break;
	case ECheckBoxState::Unchecked:
		if (AssetsToDelete.Contains(AssetData))
		{
			AssetsToDelete.Remove(AssetData);
		}
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

		if (DisplayedAssetsData.Contains(AssetData))
		{
			DisplayedAssetsData.Remove(AssetData);
		}

		RefreshListView();
	}

	return FReply::Handled();
}

FReply SAdvancedDeletionWidget::OnDeleteAllButtonClicked()
{
	if (AssetsToDelete.IsEmpty())
	{
		DebugHelper::ShowDialogMessage(EAppMsgType::Ok, TEXT("No Assets Selected."), false);
		return FReply::Handled();
	}

	// Copy over the asset data from pointer to an array
	TArray<FAssetData> AssetsToDeleteArray;
	for (const TSharedPtr<FAssetData> Data : AssetsToDelete)
	{
		AssetsToDeleteArray.Add(*Data);
	}

	const FExtendEditorManagerModule& ExtendEditorModule = FModuleManager::LoadModuleChecked<FExtendEditorManagerModule>(TEXT("ExtendEditorManager"));
	const bool bIsAssetDeleted = ExtendEditorModule.RequestMultipleDeleteAssets(AssetsToDeleteArray);

	if (bIsAssetDeleted)
	{
		AssetsData.RemoveAll([&](const TSharedPtr<FAssetData>& Item){ return AssetsToDelete.Contains(Item); });
		DisplayedAssetsData.RemoveAll([&](const TSharedPtr<FAssetData>& Item){ return AssetsToDelete.Contains(Item); });

		RefreshListView();
	}

	AssetsToDelete.Empty();
	return FReply::Handled();
}

FReply SAdvancedDeletionWidget::OnSelectAllButtonClicked()
{
	for (const auto& CheckBox : CheckBoxes)
	{
		if (!CheckBox->IsChecked())
		{
			CheckBox->ToggleCheckedState();
		}
	}
	
	return FReply::Handled();
}

FReply SAdvancedDeletionWidget::OnDeselectAllButtonClicked()
{
	for (const auto& CheckBox : CheckBoxes)
	{
		if (CheckBox->IsChecked())
		{
			CheckBox->ToggleCheckedState();
		}
	}
	
	return FReply::Handled();
}

void SAdvancedDeletionWidget::RefreshListView()
{
	CheckBoxes.Empty();
	AssetsToDelete.Empty();
	
	if (ListViewPtr.IsValid())
	{
		ListViewPtr->RebuildList();
	}
}

TSharedRef<SComboBox<TSharedPtr<FString>>> SAdvancedDeletionWidget::ConstructComboBox()
{
	ComboBoxOptions.Emplace(MakeShared<FString>(TEXT("List All Available Assets")));
	ComboBoxOptions.Emplace(MakeShared<FString>(TEXT("List All Unused Assets")));
	ComboBoxOptions.Emplace(MakeShared<FString>(TEXT("List Same Name Assets")));
	
	const TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructedComboBox = SNew(SComboBox<TSharedPtr<FString>>)
		.OptionsSource(&ComboBoxOptions)
		.OnGenerateWidget(this, &SAdvancedDeletionWidget::OnGenerateComboContent)
		.OnSelectionChanged(this, &SAdvancedDeletionWidget::OnComboSelectionChanged)
		[
			SAssignNew(ComboDisplayTextBlock, STextBlock)
			.Text(FText::FromString("List Assets Options"))
		];
	
	return ConstructedComboBox;
}

TSharedRef<SWidget> SAdvancedDeletionWidget::OnGenerateComboContent(TSharedPtr<FString> Item)
{
	const TSharedRef<STextBlock> TextBlock = SNew(STextBlock)
		.Text(FText::FromString(*Item));

	return TextBlock;
}

void SAdvancedDeletionWidget::OnComboSelectionChanged(TSharedPtr<FString> SelectedOption, ESelectInfo::Type SelectInfo)
{
	DebugHelper::ShowNotification(*SelectedOption);	ComboDisplayTextBlock->SetText(FText::FromString(*SelectedOption));

	// List all available assets
	if (*SelectedOption == *ComboBoxOptions[0])
	{
		// List all available asset data
		DisplayedAssetsData = AssetsData;
		RefreshListView();
	}
	// List all unused asset
	else if (*SelectedOption == *ComboBoxOptions[1])
	{
		// List all the unused assets
		FExtendEditorManagerModule& ExtendEditorManagerModule = FModuleManager::GetModuleChecked<FExtendEditorManagerModule>(TEXT("ExtendEditorManager"));
		ExtendEditorManagerModule.ListUnusedAssets(AssetsData, DisplayedAssetsData);
		RefreshListView();
	}
	// List Same name assets
	else if (*SelectedOption == *ComboBoxOptions[2])
	{
		FExtendEditorManagerModule& ExtendEditorManagerModule = FModuleManager::LoadModuleChecked<FExtendEditorManagerModule>(TEXT("ExtendEditorManager"));
		ExtendEditorManagerModule.ListSameNameAssets(AssetsData, DisplayedAssetsData);
		RefreshListView();
	}
}
