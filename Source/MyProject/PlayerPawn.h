// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"

UCLASS()
class MYPROJECT_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

	UPROPERTY(EditAnywhere)
	USceneComponent *OurVisibleComponent;

	void HorizontalInput(float AxisValue);
	void VerticalInput(float AxisValue);
	void JumpInput();

	//hjälp funktion
	bool Sweep(FHitResult& HitResult, const FVector& Start, const FVector& Target,
		FCollisionQueryParams& Params) const;

	void PreventCollision(float);

	//vetors 
	FVector CurrentInput = FVector(0);
	FVector JumpMovement = FVector(0);
	FVector Velocity = FVector(0);

	//vectors for bounds
	FVector Origin = FVector(0);
	FVector Extent = FVector(0);


	//editable variables
	UPROPERTY(EditAnywhere)
	float Gravity = 980;
	UPROPERTY(EditAnywhere)
	float MovementSpeed = 200;
	UPROPERTY(EditAnywhere)
	float JumpHeight = 200;
	UPROPERTY(EditAnywhere)
	float SkinWidth = 1;
	UPROPERTY(EditAnywhere)
	float GroundCheckDistance = 3;

	//helper variables 
	float SmallMovement = 1;
	int recursiveCounter = 0;
	
	
};
