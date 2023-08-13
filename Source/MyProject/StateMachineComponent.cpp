// Fill out your copyright notice in the Description page of Project Settings.


#include "StateMachineComponent.h"

#include "PlayerCharThreeD.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UStateMachineComponent::UStateMachineComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ta bort denna sen
	UStateComponent* AirStatee = CreateDefaultSubobject<UAirState>(TEXT("State"));

	AirState = CreateDefaultSubobject<UAirState>(TEXT("AirState"));
	GroundState = CreateDefaultSubobject<UGroundState>(TEXT("GroundState"));
	if (AirStatee && AirState && GroundState)
	{
		States.Add(GroundState);
		States.Add(AirState);
		//ta bortt sen
		States.Add(AirStatee);
	}
	// ...
}


// Called when the game starts
void UStateMachineComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Cast<APlayerCharThreeD>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		PlayerCharThreeD = Cast<APlayerCharThreeD>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	}
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
	UE_LOG(LogTemp, Warning, TEXT("Changing states %s"), *State->GetName());
	if (CurrentState)
		CurrentState->Exit();
	if (State)
		CurrentState = State;

	if (State)
		State->Enter();

	
}
