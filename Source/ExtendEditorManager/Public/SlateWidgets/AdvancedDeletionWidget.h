// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class EXTENDEDITORMANAGER_API SAdvancedDeletionWidget : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SAdvancedDeletionWidget) {}
		SLATE_ARGUMENT(TArray<TSharedPtr<FAssetData>>, AssetDataArray)
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& args);

private:
	TSharedRef<SListView<TSharedPtr<FAssetData>>> ConstructListView();
	TSharedRef<ITableRow> OnGenerateRowListView(TSharedPtr<FAssetData> AssetDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable);
	void OnRowWidgetClicked(TSharedPtr<FAssetData> AssetDataClicked);
	TSharedRef<SCheckBox> ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	void OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData);

	TSharedRef<STextBlock> ConstructTextBlock(const FString& TextToDisplay, const FSlateFontInfo& Font);
	TSharedRef<SButton> ConstructDeleteButton(TSharedPtr<FAssetData> AssetData);

	FReply OnDeleteButtonClicked(TSharedPtr<FAssetData> AssetData);
	FReply OnDeleteAllButtonClicked();
	FReply OnSelectAllButtonClicked();
	FReply OnDeselectAllButtonClicked();

	void RefreshListView();

	TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructComboBox();
	TSharedRef<SWidget> OnGenerateComboContent(TSharedPtr<FString> Item);
	void OnComboSelectionChanged(TSharedPtr<FString> SelectedOption, ESelectInfo::Type SelectInfo);
	
	TArray<TSharedPtr<FAssetData>> AssetsData;
	TArray<TSharedPtr<FAssetData>> DisplayedAssetsData;
	TSharedPtr<SListView<TSharedPtr<FAssetData>>> ListViewPtr;
	TSet<TSharedPtr<FAssetData>> AssetsToDelete;
	TArray<TSharedRef<SCheckBox>> CheckBoxes;
	TArray<TSharedPtr<FString>> ComboBoxOptions;
	TSharedPtr<STextBlock> ComboDisplayTextBlock;
};
