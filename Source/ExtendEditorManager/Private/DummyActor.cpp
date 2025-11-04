// Fill out your copyright notice in the Description page of Project Settings.


#include "DummyActor.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/Core/PushModel/PushModel.h"


// Sets default values
ADummyActor::ADummyActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Mark for replication
	SetReplicates(true);

	ReplicatedNumberOldWay = 10;
}

// Called when the game starts or when spawned
void ADummyActor::BeginPlay()
{
	Super::BeginPlay();

	// Change the numbers only on server
	if (HasAuthority())
	{
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle,
			[this]()
			{
				++ReplicatedNumberOldWay;
				++ReplicatedNumberOldWayCallback;

				// New way replication using push model
				SetReplicatedNumberNewWay(25);
			},
			5.0f,
			false,
			5.0f);
	}
	
}

// Called every frame
void ADummyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADummyActor::OnNumberReplicatedOldWay()
{
	UKismetSystemLibrary::PrintString(this, TEXT("Replicated Number Callback"));
}

void ADummyActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADummyActor, ReplicatedNumberOldWay);
	DOREPLIFETIME(ADummyActor, ReplicatedNumberOldWayCallback);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ADummyActor, ReplicatedNumberNewWay, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ADummyActor, ReplicatedNumberNewWayCallback, SharedParams);
}

void ADummyActor::OnNumberReplicatedNewWay()
{
	UKismetSystemLibrary::PrintString(this, TEXT("Replicated Number Callback New Way!"));
}

void ADummyActor::SetReplicatedNumberNewWay(int32 NewValue)
{
	// First change the non-callback one
	if (NewValue != ReplicatedNumberNewWay)
	{
		ReplicatedNumberNewWay = NewValue;
		MARK_PROPERTY_DIRTY_FROM_NAME(ADummyActor, ReplicatedNumberNewWay, this);
	}

	// We normally would have separate functions for each variable but let's do this here anyway
	if (NewValue != ReplicatedNumberNewWayCallback)
	{
		ReplicatedNumberNewWayCallback = NewValue;
		MARK_PROPERTY_DIRTY_FROM_NAME(ADummyActor, ReplicatedNumberNewWayCallback, this);
	}
}

