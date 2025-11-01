// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetActions/QuickActorActionWidget.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "DebugHeader.h"

bool UQuickActorActionWidget::GetEditorActorSubsystem()
{
	if (!EditorActorSubsystem)
	{
		EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	}

	return EditorActorSubsystem != nullptr;
}

void UQuickActorActionWidget::SelectAllActorsWithSimilarName()
{
	if (!GetEditorActorSubsystem())
	{
		return;
	}

	TArray<AActor*> SelectedActors = EditorActorSubsystem->GetSelectedLevelActors();
	uint32 SelectionCounter = 0;

	if (SelectedActors.IsEmpty())
	{
		DebugHelper::ShowNotification(TEXT("No Actors Selected"));
		return;
	}

	if (SelectedActors.Num() > 1)
	{
		DebugHelper::ShowNotification(TEXT("You can only select one actor."));
		return;
	}

	FString SelectedActorLabel = SelectedActors[0]->GetActorLabel();
	const FString LabelToSearch = SelectedActorLabel.LeftChop(4);

	TArray<AActor*> AllLevelActors = EditorActorSubsystem->GetAllLevelActors();

	for (const auto& LevelActor : AllLevelActors)
	{
		if (!IsValid(LevelActor))
		{
			continue;
		}

		if (LevelActor->GetActorLabel().Contains(LabelToSearch, SearchCase))
		{
			EditorActorSubsystem->SetActorSelectionState(LevelActor, true);
			SelectionCounter++;
		}
	}

	if (SelectionCounter > 0)
	{
		DebugHelper::ShowNotification(TEXT("Successfully selected ") + FString::FromInt(SelectionCounter) + TEXT(" actors"));
	}
	else
	{
		DebugHelper::ShowNotification(TEXT("No actors selected"));
	}
}

void UQuickActorActionWidget::DuplicateActors()
{
	if (!GetEditorActorSubsystem())
	{
		return;
	}

	TArray<AActor*> SelectedActors = EditorActorSubsystem->GetSelectedLevelActors();

	if (SelectedActors.IsEmpty())
	{
		DebugHelper::ShowNotification(TEXT("No Actors Selected"));
		return;
	}

	if (OffsetDistance == 0 || NumberOfDuplicates <= 0)
	{
		DebugHelper::ShowNotification(TEXT("Offset Distance or Duplicate Number is invalid."));
		return;
	}

	int32 Counter = 0;

	for (AActor* SelectedActor : SelectedActors)
	{
		if (!SelectedActor) continue;

		for (int32 i = 0; i < NumberOfDuplicates; i++)
		{
			AActor* DuplicatedActor = EditorActorSubsystem->DuplicateActor(SelectedActor, SelectedActor->GetWorld());

			if (!DuplicatedActor) continue;

			const float DuplicationOffsetDist = (i + 1) * OffsetDistance;

			switch (DuplicationAxis)
			{
			case EDuplicationAxis::XAxis:
				DuplicatedActor->AddActorWorldOffset(FVector(DuplicationOffsetDist, 0.0f, 0.0f));
				break;
			case EDuplicationAxis::YAxis:
				DuplicatedActor->AddActorWorldOffset(FVector(0.0f, DuplicationOffsetDist, 0.0f));
				break;
			case EDuplicationAxis::ZAxis:
				DuplicatedActor->AddActorWorldOffset(FVector(0.0f, 0.0f, DuplicationOffsetDist));
				break;
			case EDuplicationAxis::Max:
				break;

			default:
				break;
			}

			EditorActorSubsystem->SetActorSelectionState(DuplicatedActor, true);
			Counter++;
		}
	}

	if (Counter > 0)
	{
		DebugHelper::ShowNotification(TEXT("Successfully Duplicated ") + FString::FromInt(Counter) + TEXT(" Actors"));
	}
}
