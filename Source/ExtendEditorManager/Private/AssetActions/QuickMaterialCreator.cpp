// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetActions/QuickMaterialCreator.h"
#include  "DebugHeader.h"

void UQuickMaterialCreator::CreateMaterialFromSelectedTextures()
{
	if (bUseCustomName)
	{
		if (MaterialName.IsEmpty() || MaterialName.Equals(TEXT("M_")))
		{
			DebugHelper::ShowDialogMessage(EAppMsgType::Ok, TEXT("Please enter a valid name"), false);
			return;
		}
	}
}
