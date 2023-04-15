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

	bool Sweep(FHitResult& HitResult, const FVector& Start, const FVector& Target,
		FCollisionQueryParams& Params) const;

	//is this needed?
	UPROPERTY(EditAnywhere, Category="MyPawn|Debug")
	bool bDrawDebugActorBLACK = false;
	UPROPERTY(EditAnywhere, Category="MyPawn|Debug")
	bool bDrawDebugTraceEndRED = false;
	UPROPERTY(EditAnywhere, Category="MyPawn|Debug")
	bool bDrawDebugImpactGREEN = false;
	UPROPERTY(EditAnywhere, Category="MyPawn|Debug")
	bool bDrawDebugNormalStartMAGENTA = false;
	UPROPERTY(EditAnywhere, Category="MyPawn|Debug")
	bool bDrawDebugNormalImpactBLUE = false;
	UPROPERTY(EditAnywhere, Category="MyPawn|Debug")
	bool bDrawDebugNormalMovementORANGE = false;
	UPROPERTY(EditAnywhere, Category="MyPawn|Debug")
	float DebugLifeTime = 0.1;


	void PreventCollision(float);

	FVector CurrentInput = FVector(0);
	FVector JumpMovement = FVector(0);
	FVector Velocity = FVector(0);

	FVector Origin = FVector(0);
	FVector Extent = FVector(0);
	float SmallMovement = 1;

	UPROPERTY(EditAnywhere)
	float Gravity = 980;
	UPROPERTY(EditAnywhere)
	float MovementSpeed = 200;
	UPROPERTY(EditAnywhere)
	bool bJump; 
	UPROPERTY(EditAnywhere)
	float JumpHeight = 200;
	UPROPERTY(EditAnywhere)
	float SkinWidth = 1;
	UPROPERTY(EditAnywhere)
	float GroundCheckDistance = 3;

	int recursiveCounter = 0;
	
	
};