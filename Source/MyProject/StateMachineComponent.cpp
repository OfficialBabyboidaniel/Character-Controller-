// Fill out your copyright notice in the Description page of Project Settings.


#include "StateMachineComponent.h"

// Sets default values for this component's properties
UStateMachineComponent::UStateMachineComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	UStateComponent* AirStatee = CreateDefaultSubobject<UAirState>(TEXT("State"));
	/*GroundState = CreateDefaultSubobject<UGroundState>(TEXT("AirState"));
	AirState = CreateDefaultSubobject<UAirState>(TEXT("AirState"));
	if (AirState && GroundState)
	{
		States.Add(GroundState);
		States.Add(AirState);
	}*/
	AirState = CreateDefaultSubobject<UAirState>(TEXT("AirState"));
	if (AirStatee && AirState)
	{
		States.Add(AirState);
		States.Add(AirStatee);
	}
	// ...
}


// Called when the game starts
void UStateMachineComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UStateMachineComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UStateMachineComponent::ChangeState(UStateComponent* State)
{
	UE_LOG(LogTemp, Warning, TEXT("Changing states"));
	if (State)
		State->Enter();
}
