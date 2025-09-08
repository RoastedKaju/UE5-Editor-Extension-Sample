// Fill out your copyright notice in the Description page of Project Settings.


#include "SlateWidgets/AdvancedDeletionWidget.h"

void SAdvancedDeletionWidget::Construct(const FArguments& args)
{
	bCanSupportFocus = true;

	FSlateFontInfo FontInfo = FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
	FontInfo.Size = 30;

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Advanced Deletion"))
			.Font(FontInfo)
			.Justification(ETextJustify::Center)
			.ColorAndOpacity(FColor::White)
		]
	];
}
