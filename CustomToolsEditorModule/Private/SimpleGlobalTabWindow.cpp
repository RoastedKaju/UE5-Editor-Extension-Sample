// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleGlobalTabWindow.h"

#define LOCTEXT_NAMESPACE "SimpleTabWindow"

void SimpleGlobalTabWindow::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("SimpleTab_Header", "Hello World!"))
			.Font(FCoreStyle::GetDefaultFontStyle("Italic", 48))
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5)
			[
				SNew(SButton)
				.Text(LOCTEXT("SimpleTab_Button1", "OK"))
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5)
			[
				SNew(SButton)
				.Text(LOCTEXT("SimpleTab_Button2", "Cancel"))
			]
		]
	];
}

#undef LOCTEXT_NAMESPACE
