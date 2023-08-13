// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateComponent.h"
#include "GroundState.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UGroundState : public UStateComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGroundState();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	virtual void Update(float DeltaTime) override;

private:
	//Vectors values
	FVector CurrentInput;
	FVector Velocity = FVector::ZeroVector;
	FVector Gravity;

	//camera properties
	UPROPERTY(EditAnywhere, Category="Camera Stats")
	class UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere, Category="Camera Stats")
	FVector CameraLocationRelativeToPlayer;

	//camera rotation
	UPROPERTY(VisibleAnywhere, Category="Camera Stats")
	FQuat CameraRotation;
	UPROPERTY(VisibleAnywhere, Category="Camera Stats")
	FVector EulerRotation;
	UPROPERTY(EditAnywhere, Category="Camera Stats")
	double MaxPitchRotaion = 30;
	UPROPERTY(EditAnywhere, Category="Camera Stats")
	double MinPitchRotation = -50;

	//camera input and variables
	UPROPERTY(VisibleAnywhere, Category="Camera Stats")
	FVector CameraInput = FVector::ZeroVector;
	UPROPERTY(VisibleAnywhere, Category="Camera Stats")
	double OffsetDistance;
	UPROPERTY(EditAnywhere, Category="Camera Stats")
	double MouseSensitivity = 2;
	UPROPERTY(EditAnywhere, Category="Camera Stats")
	double CameraSkinWidth = 5;

	//Sweep values
	FVector Origin, Extent;
	FCollisionQueryParams Params;

	//player stats
	UPROPERTY(EditAnywhere, Category="Stats")
	float Acceleration = 500.0f;
	UPROPERTY(EditAnywhere, Category="Stats")
	float MaxSpeed = 2500.0f;
	UPROPERTY(EditAnywhere, Category="Stats")
	float SkinWidth = 1.2f;

	//jump variables
	UPROPERTY(EditAnywhere, Category="Stats")
	float JumpForce = 400.0f;
	UPROPERTY(EditAnywhere, Category="Stats")
	float GroundCheckDistance = 1.5f;

	//Physics values
	UPROPERTY(EditAnywhere, Category="Stats")
	float GravityForce = 300.0f;
	UPROPERTY(EditAnywhere, Category="Stats")
	double StaticFrictionCoefficient = 0.6;
	UPROPERTY(EditAnywhere, Category="Stats")
	double KineticFrictionCoefficient = 0.4;
	UPROPERTY(EditAnywhere, Category="Stats")
	double AirResistanceCoefficient = 0.6;

	//state values
	UPROPERTY(VisibleAnywhere, Category="State")
	class UStateComponent* CurrentState;
	UPROPERTY(VisibleAnywhere, Category="State")
	class UStateMachineComponent* StateMachineComponent;

	// for update vel function
	int RecursivCounter = 0;
	//functions
	void UpdateVelocity(float DeltaTime);
	void CalculateInput(float DeltaTime);
	void ApplyFriction(float DeltaTime, float NormalMagnitude);
	
	//help functions
	void CalculatePitchInput();
	void SetInitialCameraLocation(float DeltaTime);
	void CameraCollisionCheck();
	void CalculateInitialVelocity(float DeltaTime);
};
