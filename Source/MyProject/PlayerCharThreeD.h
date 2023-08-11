// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "State.h"
#include "GameFramework/Character.h"
#include "PlayerCharThreeD.generated.h"

UCLASS()
class MYPROJECT_API APlayerCharThreeD : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharThreeD();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
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

	//camera values
	UPROPERTY(EditAnywhere, Category="Camera Stats")
	class UCameraComponent* Camera;
	UPROPERTY(EditAnywhere, Category="Camera Stats")
	double MouseSensitivity = 2;
	UPROPERTY(EditAnywhere, Category="Camera Stats")
	double MaxPitchRotaion = 70;
	UPROPERTY(EditAnywhere, Category="Camera Stats")
	double MinPitchRotation = -30;
	UPROPERTY(VisibleAnywhere, Category="Camera Stats")
	FQuat CameraRotation;
	UPROPERTY(VisibleAnywhere, Category="Camera Stats")
	FVector EulerRotation;
	UPROPERTY(VisibleAnywhere, Category="Camera Stats")
	FVector CameraLocationRelativeToPlayer;
	UPROPERTY(VisibleAnywhere, Category="Camera Stats")
	FVector CameraInput = FVector::ZeroVector;
	UPROPERTY(VisibleAnywhere, Category="Camera Stats")
	FVector RotationY;
	UPROPERTY(VisibleAnywhere, Category="Camera Stats")
	FVector RotationZ;
	UPROPERTY(VisibleAnywhere, Category="Camera Stats")
	double YawAxisValue;
	UPROPERTY(VisibleAnywhere, Category="Camera Stats")
	double PitchAxisValue;
	UPROPERTY(VisibleAnywhere, Category="Camera Stats")
	double OffsetDistance;


	//Sweep values
	FVector Origin, Extent;
	FCollisionQueryParams Params;

	//stats
	UPROPERTY(EditAnywhere, Category="Stats")
	float Acceleration = 500.0f;
	UPROPERTY(EditAnywhere, Category="Stats")
	float MaxSpeed = 2500.0f;
	UPROPERTY(EditAnywhere, Category="Stats")
	float SkinWidth = 1.2f;
	UPROPERTY(EditAnywhere, Category="Stats")
	float GroundCheckDistance = 1.5f;
	UPROPERTY(EditAnywhere, Category="Stats")
	float GravityForce = 300.0f;
	UPROPERTY(EditAnywhere, Category="Stats")
	float JumpForce = 400.0f;
	UPROPERTY(EditAnywhere, Category="Stats")
	double StaticFrictionCoefficient = 0.6;
	UPROPERTY(EditAnywhere, Category="Stats")
	double KineticFrictionCoefficient = 0.4;
	UPROPERTY(EditAnywhere, Category="Stats")
	double AirResistanceCoefficient = 0.6;

	UPROPERTY(VisibleAnywhere, Category="State")
	//class State CurrentState;
	
	
	int RecursivCounter = 0;

	//functions
	void UpdateVelocity(float DeltaTime);
	void CalculateInput(float DeltaTime);
	// kanske behövs göras om till  vector value ist för float
	void ApplyFriction(float DeltaTime, float NormalMagnitude);
};
