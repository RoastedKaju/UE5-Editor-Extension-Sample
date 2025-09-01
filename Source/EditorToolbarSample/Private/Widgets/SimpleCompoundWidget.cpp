// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/SimpleCompoundWidget.h"

#define LOCTEXT_NAMESPACE "SimpleCompoundWidget"

void SimpleCompoundWidget::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("Greetings", "Hello Simple Compound Widget."))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 48))
		]
	];
}

#undef LOCTEXT_NAMESPACE
