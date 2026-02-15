// // Copyright (C) 2025 R.Haris Nauman. All Rights Reserved.


#include "Commands/ExtendEditorUICommands.h"

#define LOCTEXT_NAMESPACE "ExtendEditorManager"

void FExtendEditorUICommands::RegisterCommands()
{
	UI_COMMAND(
		LockActorSelection,
		"Lock Actor Selection",
		"Actor Can No Longer be selected.",
		EUserInterfaceActionType::Button,
		FInputChord(EKeys::W, EModifierKey::Alt));

	UI_COMMAND(
		UnlockActorSelection,
		"Unlock Actor Selection",
		"Actor Can be selected Again.",
		EUserInterfaceActionType::Button,
		FInputChord(EKeys::W, EModifierKey::Alt | EModifierKey::Shift));
}

#undef LOCTEXT_NAMESPACE
