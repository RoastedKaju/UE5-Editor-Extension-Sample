// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "QuickActorActionWidget.generated.h"

class UEditorActorSubsystem;

/**
 * 
 */
UCLASS()
class EXTENDEDITORMANAGER_API UQuickActorActionWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	bool GetEditorActorSubsystem();

	UFUNCTION(BlueprintCallable)
	void SelectAllActorsWithSimilarName();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBatchSelection")
	TEnumAsByte<ESearchCase::Type> SearchCase = ESearchCase::IgnoreCase;

private:
UPROPERTY()
	UEditorActorSubsystem* EditorActorSubsystem;
};
