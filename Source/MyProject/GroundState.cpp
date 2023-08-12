// Fill out your copyright notice in the Description page of Project Settings.


#include "GroundState.h"

UGroundState::UGroundState()
{
}

void UGroundState::BeginPlay()
{
	Super::BeginPlay();
}

void UGroundState::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(bShouldTick)
	{
	Update();	
	}
}

void UGroundState::Update()
{
	Super::Update();
	
	UE_LOG(LogTemp, Warning, TEXT("groundState ticking"));
}
