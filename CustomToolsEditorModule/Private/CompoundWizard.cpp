// Fill out your copyright notice in the Description page of Project Settings.


#include "CompoundWizard.h"

#define LOCTEXT_NAMESPACE "CompoundWizardWindow"

void CompoundWizard::Construct(const FArguments& args)
{
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("CompoundWizardWindow", "Compound Wizard Window"))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 48))
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10)
		[
			SNew(SButton)
			.Text(LOCTEXT("CompoundWizardWindow", "Close"))
			.HAlign(HAlign_Center)
			.OnClicked_Lambda([WeakThis = TWeakPtr<SWidget>(AsShared())]()
			{
				if (WeakThis.IsValid())
				{
					TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow(WeakThis.Pin().ToSharedRef());
					if (ParentWindow.IsValid())
					{
						UE_LOG(LogTemp, Log, TEXT("Destroy Window Called."));
						ParentWindow->RequestDestroyWindow();
					}
				}
				return FReply::Handled();
			})
		]
	];
}

#undef LOCTEXT_NAMESPACE
