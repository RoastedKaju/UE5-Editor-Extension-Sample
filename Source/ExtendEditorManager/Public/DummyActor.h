// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DummyActor.generated.h"

UCLASS()
class EXTENDEDITORMANAGER_API ADummyActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADummyActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, Replicated)
	int32 ReplicatedNumberOldWay;

	UFUNCTION()
	void OnNumberReplicatedOldWay();

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnNumberReplicatedOldWay)
	int32 ReplicatedNumberOldWayCallback;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadWrite, Replicated)
	int32 ReplicatedNumberNewWay;

	UFUNCTION()
	void OnNumberReplicatedNewWay();

	// This function will be used to change the value and mark the variables dirty on server
	UFUNCTION()
	void SetReplicatedNumberNewWay(int32 NewValue);
	
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing= OnNumberReplicatedNewWay)
	int32 ReplicatedNumberNewWayCallback;
};
