// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharThreeD.generated.h"

class UCameraComponent;
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
	//virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

//get and set functions
	FVector GetCurrentInput() const;
	FVector GetJumpInput() const;
	float GetPitchAxisValue() const;
	float GetYawAxisValue() const;
	void SetCurrentInput(const FVector NewValue);
	void SetJumpInput(const FVector NewValue);
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

	UPROPERTY(VisibleAnywhere, Category="Camera Stats")
	double YawAxisValue;
	UPROPERTY(VisibleAnywhere, Category="Camera Stats")
	double PitchAxisValue;

	//Sweep values
	FVector Origin, Extent;
	FCollisionQueryParams Params;

	//jump info 
	UPROPERTY(EditAnywhere, Category="Stats")
	float GroundCheckDistance = 1.5f;
	UPROPERTY(EditAnywhere, Category="Stats")
	float JumpForce = 400.0f;
	UPROPERTY(EditAnywhere, Category="Stats")
	float SkinWidth = 1.2f;
	
	UPROPERTY(VisibleAnywhere, Category="State")
	class UStateComponent* CurrentState;
	UPROPERTY(VisibleAnywhere, Category="State")
	class UStateMachineComponent* StateMachineComponent;
};

