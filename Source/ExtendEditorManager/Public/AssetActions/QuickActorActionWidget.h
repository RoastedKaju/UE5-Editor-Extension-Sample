// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "QuickActorActionWidget.generated.h"

class UEditorActorSubsystem;

UENUM(BlueprintType)
enum class EDuplicationAxis : uint8
{
	XAxis		UMETA(DisplayName = "X-Axis"),
	YAxis		UMETA(DisplayName = "Y-Axis"),
	ZAxis		UMETA(DisplayName = "Z-Axis"),
	Max			UMETA(DisplayName = "Max")
};

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

	UFUNCTION(BlueprintCallable)
	void DuplicateActors();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorDuplication")
	EDuplicationAxis DuplicationAxis = EDuplicationAxis::XAxis;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorDuplication")
	float OffsetDistance = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorDuplication")
	int32 NumberOfDuplicates = 3;	

private:
UPROPERTY()
	UEditorActorSubsystem* EditorActorSubsystem;
};
