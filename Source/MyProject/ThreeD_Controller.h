// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ThreeD_Controller.generated.h"

UCLASS()
class MYPROJECT_API AThreeD_Controller : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AThreeD_Controller();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UPROPERTY(EditAnywhere)
	USceneComponent* OurVisibleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class USpringArmComponent* CameraBoom;

	// The main camera component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* CameraComponent;
	

	void XInput(float AxisValue);
	void YInput(float AxisValue);
	void JumpInput();
	void LookRight(float AxisValue);
	void LookUp(float AxisValue);

	
	//Vectors values
	FVector CurrentInput;
	FVector Velocity = FVector::ZeroVector;
	FVector JumpMovement;
	FVector Gravity;

	//distance value
	double Distance;
	
	//Sweep values
	FVector Origin, Extent;
	FCollisionQueryParams Params;

	//stats
	UPROPERTY(EditAnywhere, Category="Stats")
	float RotationSpeed = 30.0f;
	
	UPROPERTY(EditAnywhere, Category="Stats")
	float Acceleration = 500.0f;
	UPROPERTY(EditAnywhere, Category="Stats")
	float MaxSpeed = 2500.0f;
	UPROPERTY(EditAnywhere, Category="Stats")
	float SkinWidth = 1.2f;
	UPROPERTY(EditAnywhere, Category="Stats")
	float GroundCheckDistance = 0.5f;
	UPROPERTY(EditAnywhere, Category="Stats")
	float GravityForce = 300.0f;
	UPROPERTY(EditAnywhere, Category="Stats")
	float JumpForce = 400.0f;
	UPROPERTY(EditAnywhere, Category="Stats")
	int RecursivCounter = 0;
	UPROPERTY(EditAnywhere, Category="Stats")
	double StaticFrictionCoefficient = 0.6;
	UPROPERTY(EditAnywhere, Category="Stats")
	double KineticFrictionCoefficient  = 0.4;
	UPROPERTY(EditAnywhere, Category="Stats")
	double  AirResistanceCoefficient  = 0.6;


	//functions
	void UpdateVelocity(float DeltaTime);
	void CalculateInput(float DeltaTime);
	// kanske behövs göras om till  vector value ist för float
	void ApplyFriction(float DeltaTime, float NormalMagnitude);
};
