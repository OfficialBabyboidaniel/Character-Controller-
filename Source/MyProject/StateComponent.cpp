// Fill out your copyright notice in the Description page of Project Settings.


#include "StateComponent.h"

#include "PlayerCharThreeD.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UStateComponent::UStateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UStateComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Cast<APlayerCharThreeD>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		PlayerCharThreeD = Cast<APlayerCharThreeD>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	}
}


// Called every frame
void UStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

void UStateComponent::Enter()
{
	bShouldTick = true;
}

void UStateComponent::Update(float DeltaTime)
{
	
}

void UStateComponent::Exit()
{
	bShouldTick = false;
}
