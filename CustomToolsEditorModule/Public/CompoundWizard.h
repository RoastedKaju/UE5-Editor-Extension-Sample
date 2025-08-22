// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class CUSTOMTOOLSEDITORMODULE_API CompoundWizard : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(CompoundWizard)
		{
		}

	SLATE_END_ARGS()

	void Construct(const FArguments& args);
};
