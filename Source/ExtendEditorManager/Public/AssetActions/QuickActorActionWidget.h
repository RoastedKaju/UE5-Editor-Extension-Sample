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

USTRUCT(BlueprintType)
struct FRandomActorRotation
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRotateYaw = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = bRotateYaw))
	float YawMinRotation = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = bRotateYaw))
	float YawMaxRotation = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRotateRoll = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = bRotateYaw))
	float RollMinRotation = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = bRotateYaw))
	float RollMaxRotation = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRotatePitch = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = bRotateYaw))
	float PitchMinRotation = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = bRotateYaw))
	float PitchMaxRotation = 180.0f;
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

	UFUNCTION(BlueprintCallable)
	void RandomizeActorRotation();

	UFUNCTION(BlueprintCallable)
	void RandomizeActorOffsetAndScale();

	UFUNCTION(BlueprintCallable)
	void DuplicateActors();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBatchSelection")
	TEnumAsByte<ESearchCase::Type> SearchCase = ESearchCase::IgnoreCase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorDuplication")
	EDuplicationAxis DuplicationAxis = EDuplicationAxis::XAxis;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorDuplication")
	float OffsetDistance = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorDuplication")
	int32 NumberOfDuplicates = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorRotation")
	FRandomActorRotation RandomRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorRandomization")
	bool bRandomizeScale = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorRandomization", meta = (EditCondition = bRandomizeScale))
	float MinScale = 0.8f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorRandomization", meta = (EditCondition = bRandomizeScale))
	float MaxScale = 1.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorRandomization")
	bool bRandomizeOffset = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorRandomization", meta = (EditCondition = bRandomizeOffset))
	float OffsetMin = -50.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorRandomization", meta = (EditCondition = bRandomizeOffset))
	float OffsetMax = 50.0f;

private:
UPROPERTY()
	UEditorActorSubsystem* EditorActorSubsystem;
};
