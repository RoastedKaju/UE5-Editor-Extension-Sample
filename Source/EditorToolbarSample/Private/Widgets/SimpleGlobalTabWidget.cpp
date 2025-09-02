// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/SimpleGlobalTabWidget.h"

void SimpleGlobalTabWidget::Construct(const FArguments& args)
{
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.Padding(10)
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Hello Global Tab Widget"))
			.Font(FCoreStyle::GetDefaultFontStyle("Italic", 35))
		]
	];
}
