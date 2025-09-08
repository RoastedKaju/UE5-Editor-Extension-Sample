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
		SLATE_ARGUMENT(FString, TestString)
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& args);
};
