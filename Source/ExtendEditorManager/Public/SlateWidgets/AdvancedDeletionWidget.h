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
	TSharedRef<ITableRow> OnGenerateRowListView(TSharedPtr<FAssetData> AssetDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable);
	TSharedRef<SCheckBox> ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	void OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData);
	
	TArray<TSharedPtr<FAssetData>> AssetsData;
};
