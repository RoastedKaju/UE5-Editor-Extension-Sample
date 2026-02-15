// // Copyright (C) 2025 R.Haris Nauman. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"


class FExtendEditorUICommands : public TCommands<FExtendEditorUICommands>
{
public:
	FExtendEditorUICommands() : TCommands<FExtendEditorUICommands>(
	TEXT("ExtendEditor"),
	FText::FromString(TEXT("Extend Editor UI Commands")),
	NAME_None,
	TEXT("ExtendEditor")
	) {}

	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> LockActorSelection;
	TSharedPtr<FUICommandInfo> UnlockActorSelection;
};