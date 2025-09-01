// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/SimpleCompoundWidget.h"

#define LOCTEXT_NAMESPACE "SimpleCompoundWidget"

void SSimpleCompoundWidget::Construct(const FArguments& InArgs)
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
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10)
		.HAlign(HAlign_Center)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5)
			[
				SNew(SButton)
				.Text(LOCTEXT("FPS", "Toggle FPS"))
				.HAlign(HAlign_Center)
				.OnClicked_Lambda([]()
				{
					GEngine->Exec(nullptr, TEXT("stat fps"));
					return FReply::Handled();
				})
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5)
			[
				SNew(SButton)
				.Text(LOCTEXT("Quit", "Quit Window"))
				.HAlign(HAlign_Center)
				.OnClicked_Lambda([WeakThis = TWeakPtr<SWidget>(AsShared())]()
				{
					if (WeakThis.IsValid())
					{
						const TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow(WeakThis.Pin().ToSharedRef());
						if (ParentWindow.IsValid())
						{
							ParentWindow->RequestDestroyWindow();
						}
					}
					return FReply::Handled();
				})
			]
		]
	];
}

#undef LOCTEXT_NAMESPACE
