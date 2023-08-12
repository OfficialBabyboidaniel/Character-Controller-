// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateComponent.h"
#include "AirState.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UAirState : public UStateComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAirState();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	virtual void Update(float DeltaTime) override;
};
