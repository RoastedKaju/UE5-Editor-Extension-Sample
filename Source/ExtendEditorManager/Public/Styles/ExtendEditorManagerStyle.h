// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

class FExtendEditorManagerStyle
{
public:
	static void InitializeIcons();
	static void Shutdown();

private:
	static FName StyleSetName;
	static TSharedPtr<FSlateStyleSet> CreatedSlateStyle;
	static TSharedRef<FSlateStyleSet> CreateSlateStyleSet();
};