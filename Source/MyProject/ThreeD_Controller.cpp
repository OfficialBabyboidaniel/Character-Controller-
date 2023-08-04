// Fill out your copyright notice in the Description page of Project Settings.


#include "ThreeD_Controller.h"
#include <Windows.Graphics.Display.h>

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Sampling/NormalHistogram.h"
#include "StaticHelperClass.h"
#include "Styling/StyleColors.h"
#include "Util/ColorConstants.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h" // Include the SpringArmComponent header
#include "Camera/CameraComponent.h"

// Sets default values
AThreeD_Controller::AThreeD_Controller()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;
	// create root component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	// create mesh
	OurVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OurVisibleComponent"));

	OurVisibleComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AThreeD_Controller::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AThreeD_Controller::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//DrawDebugBox(GetWorld(), Origin, Extent, FColor::Red, true);
	Gravity = FVector::DownVector * GravityForce * DeltaTime;

	//addera jump och gravity 
	Velocity += Gravity + JumpMovement;
	//calculera input.
	CalculateInput(DeltaTime);
	//multiplicera med luftmotstånd
	Velocity *= FMath::Pow(AirResistanceCoefficient, DeltaTime);
	UpdateVelocity(DeltaTime);

	const FVector CurrentLocation = GetActorLocation();
	SetActorLocation(CurrentLocation + Velocity * DeltaTime);
	JumpMovement = FVector::ZeroVector;
}

// Called to bind functionality to input
void AThreeD_Controller::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// bind inputs
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AThreeD_Controller::JumpInput);
	PlayerInputComponent->BindAxis("X", this, &AThreeD_Controller::XInput);
	PlayerInputComponent->BindAxis("Y", this, &AThreeD_Controller::YInput);
	PlayerInputComponent->BindAxis("LookRight", this, &AThreeD_Controller::LookRight);
	PlayerInputComponent->BindAxis("LookUp", this, &AThreeD_Controller::LookUp);
}
// watch a real tutorial 
void AThreeD_Controller::XInput(float AxisValue)
{
	/*// Ensure that the player controller is valid
	if (APlayerController* MyController = GetController<APlayerController>())
	{
		// Get the camera manager from the player controller
		APlayerCameraManager* CameraManager = MyController->PlayerCameraManager;

		if (CameraManager)
		{
			// Get the camera's forward vector (the direction it is facing)
			FVector CameraForward = CameraManager->GetCameraRotation().Vector();

			// Flatten the camera's forward vector, so we ignore the Z (up/down) component
			CameraForward.Z = 0.0f;
			CameraForward.Normalize();

			FVector CameraRight = CameraForward.RotateAngleAxis(180.0f, FVector::UpVector);

			// Set the forward movement direction based on the camera's forward vector
			FVector ForwardDirection = CameraRight * AxisValue;

			// Update the input vector
			CurrentInput = FVector(AxisValue, CurrentInput.Y, CurrentInput.Z);
		}
	}
	CurrentInput = FVector(AxisValue, CurrentInput.Y, CurrentInput.Z);*/
}


// vertical axis input
void AThreeD_Controller::YInput(float AxisValue)
{
	/*if (APlayerController* MyController = GetController<APlayerController>())
	{
		// Get the camera manager from the player controller
		APlayerCameraManager* CameraManager = MyController->PlayerCameraManager;

		if (CameraManager)
		{
			// Get the camera's forward vector (the direction it is facing)
			FVector CameraForward = CameraManager->GetCameraRotation().Vector();

			// Flatten the camera's forward vector, so we ignore the Z (up/down) component
			CameraForward.Z = 0.0f;
			CameraForward.Normalize();

			// Get the camera's right vector (the direction perpendicular to forward)
			FVector CameraRight = CameraForward.RotateAngleAxis(90.0f, FVector::UpVector);

			// Set the rightward movement direction based on the camera's right vector
			FVector RightDirection = CameraRight * AxisValue;

			// Update the input vector
			CurrentInput = FVector(RightDirection.X, RightDirection.Y, CurrentInput.Z);
		}
	}*/
	// Ensure that the player controller is valid
}


void AThreeD_Controller::LookRight(float AxisValue)
{
	UE_LOG(LogTemp, Warning, TEXT("Tryin to look rihgt"));
	AddControllerYawInput(AxisValue * RotationSpeed * GetWorld()->GetDeltaSeconds());
}

void AThreeD_Controller::LookUp(float AxisValue)
{
	UE_LOG(LogTemp, Warning, TEXT("Tryin to look up"));
	AddControllerPitchInput(AxisValue * RotationSpeed * GetWorld()->GetDeltaSeconds());
}


// jump input
void AThreeD_Controller::JumpInput()
{
	// kolla igenom sen?
	GetActorBounds(true, Origin, Extent);
	FHitResult Hit;
	FVector TraceEnd = Origin + FVector::DownVector * (GroundCheckDistance + SkinWidth);
	Params.AddIgnoredActor(this);
	//använda get actor location? 
	bool bHit = GetWorld()->SweepSingleByChannel(Hit, Origin, TraceEnd, FQuat::Identity, ECC_Pawn,
	                                             FCollisionShape::MakeBox(Extent), Params);
	if (bHit)
	{
		JumpMovement = FVector(0, 0, JumpForce);
	}
}


void AThreeD_Controller::UpdateVelocity(float DeltaTime)
{
	GetActorBounds(true, Origin, Extent);
	FHitResult Hit;
	FHitResult NormalHit;
	FVector TraceStart = Origin;
	FVector TraceEnd = Origin + Velocity.GetSafeNormal() * Velocity.Size() * DeltaTime + SkinWidth;
	Params.AddIgnoredActor(this);
	bool bHit;
	bHit = GetWorld()->SweepSingleByChannel(
		Hit,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeBox(Extent),
		Params);

	DrawDebugLine(
		GetWorld(),
		TraceStart,
		TraceEnd,
		FColor::Red, // Line color (you can change this to any color you like)
		false, // Persistent (false means the line will disappear after one frame)
		5.0f, // LifeTime (negative value means the line will stay forever)
		0, // DepthPriority (you can adjust this if necessary)
		2.0f // Thickness (you can adjust this to change the line's thickness)
	);
	if (bHit)
	{
		TraceEnd = Origin - Hit.Normal * Hit.Distance;
		bHit = GetWorld()->SweepSingleByChannel(NormalHit, TraceStart, TraceEnd, FQuat::Identity, ECC_Pawn,
		                                        FCollisionShape::MakeBox(Extent), Params);
		//flytta actor mto normalen av träffpunkten
		SetActorLocation(GetActorLocation() - Hit.Normal * (NormalHit.Distance - SkinWidth));
	}

	if (Velocity.Size() /** GetWorld()->DeltaTimeSeconds*/ < 0.1)
	{
		RecursivCounter = 0;
		//ska  det vara zero vector här?
		Velocity = FVector::ZeroVector;
		return;
	}

	if (RecursivCounter > 10)
	{
		UE_LOG(LogTemp, Warning, TEXT("Recursive counter 10"));
		RecursivCounter = 0;
		Velocity = FVector::ZeroVector;
	}
	// UE_LOG(LogTemp, Warning, TEXT("Counter %d"), RecursivCounter);
	if (bHit)
	{
		//dubbel kolla normal kraft beräkning, det gungar ftf, kan ej vara helt still.  
		FVector NormalPower = StaticHelperClass::DotProduct(Velocity, Hit.ImpactNormal);
		//skriva kod här


		Velocity += NormalPower /*+ NormalHit.ImpactNormal * SkinWidth*/;

		RecursivCounter++;

		ApplyFriction(DeltaTime, NormalPower.Size());
		UpdateVelocity(DeltaTime);
	}
	RecursivCounter = 0;
}

void AThreeD_Controller::CalculateInput(float DeltaTime)
{
	Velocity += CurrentInput.GetSafeNormal() * Acceleration * DeltaTime;
}

void AThreeD_Controller::ApplyFriction(float DeltaTime, float NormalMagnitude)
{
	if (Velocity.Size() <
		NormalMagnitude * StaticFrictionCoefficient)
	{
		Velocity = FVector::ZeroVector;
	}
	else
	{
		Velocity -= Velocity.GetSafeNormal() * NormalMagnitude *
			KineticFrictionCoefficient;
	}
}
