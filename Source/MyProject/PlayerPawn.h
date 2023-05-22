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

	//Inputs
	void HorizontalInput(float AxisValue);
	void VerticalInput(float AxisValue);
	void JumpInput();

	//Vectors values
	FVector CurrentInput;

	//Sweep values
	FVector Origin, Extent;
	FCollisionQueryParams Params;

	//stats
	UPROPERTY(EditAnywhere, Category="Stats")
	float MovementSpeed = 600.0f;
	UPROPERTY(EditAnywhere, Category="Stats")
	float SkinWidth = 1.0f;
	UPROPERTY(EditAnywhere, Category="Stats")
	float GroundCheckDistance = 0.5f; 
	UPROPERTY(EditAnywhere, Category="Stats")
	float GravityForce = 500.0f;
	UPROPERTY(EditAnywhere, Category="Stats")
	float JumpForce = 100.0f;
	
	FVector JumpMovement;	
	
	//functions
	FVector CollisionFunction(FVector Movement);
	
};
