// Fill out your copyright notice in the Description page of Project Settings.
#include "PlayerCharThreeD.h"
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
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/PrimitiveComponent.h"
//#include "Camera/Component.h"

// Sets default values
APlayerCharThreeD::APlayerCharThreeD()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void APlayerCharThreeD::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlayerCharThreeD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Gravity = FVector::DownVector * GravityForce * DeltaTime;

	//addera jump och gravity 
	Velocity += Gravity + JumpMovement;
	//calculera input.
	CalculateInput(DeltaTime);
	//multiplicera med luftmotstånd
	Velocity *= FMath::Pow(AirResistanceCoefficient, DeltaTime);

	// Get the current time before calling UpdateVelocity
	const double StartTime = FPlatformTime::Seconds();

	// Call the UpdateVelocity function
	UpdateVelocity(DeltaTime);

	// Get the current time after UpdateVelocity has finished
	const double EndTime = FPlatformTime::Seconds();

	// Calculate the time taken by UpdateVelocity
	const double TimeTaken = EndTime - StartTime;

	// Subtract the time taken by UpdateVelocity from DeltaTime
	const float AdjustedDeltaTime = FMath::Max(DeltaTime - TimeTaken, 0.0f);
	if (Velocity.Size() > MaxSpeed)
	{
		Velocity = Velocity.GetClampedToMaxSize(MaxSpeed);
	}
	const FVector CurrentLocation = GetActorLocation();
	SetActorLocation(CurrentLocation + Velocity * AdjustedDeltaTime);
	UE_LOG(LogTemp, Warning, TEXT("Velocity with delta time : %s"), *Velocity.ToString()); // Log Velocity
	JumpMovement = FVector::ZeroVector;

	/*FQuat CameraRotation = FQuat::MakeFromEuler(CameraInput);
	CameraRotation *= DeltaTime;
	SetActorRotation(CameraRotation);*/
	/*CameraInput = FVector::ZeroVector;*/
}

// Called to bind functionality to input
void APlayerCharThreeD::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// bind inputs
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharThreeD::JumpInput);
	PlayerInputComponent->BindAxis("X", this, &APlayerCharThreeD::XInput);
	PlayerInputComponent->BindAxis("Y", this, &APlayerCharThreeD::YInput);
	PlayerInputComponent->BindAxis("LookRight", this, &APlayerCharThreeD::LookRight);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharThreeD::LookUp);
}


void APlayerCharThreeD::XInput(float AxisValue)
{
	// is clamp needed? 
	//	CurrentInput = FVector(FMath::Clamp<float>(AxisValue, -1.0f, 1.0f), CurrentInput.Y, CurrentInput.Z);
	CurrentInput = FVector(AxisValue, CurrentInput.Y, CurrentInput.X);
}


// vertical axis input
void APlayerCharThreeD::YInput(float AxisValue)
{
	// is clamp needed
	//CurrentInput = FVector(CurrentInput.X, FMath::Clamp<float>(AxisValue, -1.0f, 1.0f), CurrentInput.Z);
	CurrentInput = FVector(CurrentInput.X, AxisValue, CurrentInput.Z);
}


void APlayerCharThreeD::LookRight(float AxisValue)
{
	YawAxisValue = AxisValue;
	CameraInput.X += YawAxisValue * MouseSensitivity;
	//AddControllerYawInput(AxisValue * RotationSpeed * GetWorld()->GetDeltaSeconds());
}

void APlayerCharThreeD::LookUp(float AxisValue)
{
	PitchAxisValue = AxisValue;
	CameraInput.Y += PitchAxisValue * MouseSensitivity;
	//AddControllerPitchInput(AxisValue * RotationSpeed * GetWorld()->GetDeltaSeconds());
}


// jump input
void APlayerCharThreeD::JumpInput()
{
	GetActorBounds(true, Origin, Extent);
	FHitResult Hit;
	FVector TraceEnd = Origin + FVector::DownVector * (GroundCheckDistance + SkinWidth);
	Params.AddIgnoredActor(this);
	bool bHit = GetWorld()->SweepSingleByChannel(Hit, Origin, TraceEnd, FQuat::Identity, ECC_Pawn,
	                                             FCollisionShape::MakeCapsule(Extent), Params);
	if (bHit)
	{
		JumpMovement = FVector(0, 0, JumpForce);
	}
	UE_LOG(LogTemp, Warning, TEXT("Jumpiong"));
}


void APlayerCharThreeD::UpdateVelocity(float DeltaTime)
{
	GetActorBounds(true, Origin, Extent);
	FHitResult Hit;
	FHitResult NormalHit;
	FVector TraceStart = Origin;
	FVector TraceEnd = Origin + Velocity.GetSafeNormal() * (Velocity.Size() + SkinWidth) * DeltaTime;
	TArray<FOverlapResult> OverlapResult;
	Params.AddIgnoredActor(this);
	bool bHit;
	bool bHit2;
    	bHit = GetWorld()->SweepSingleByChannel(
		Hit,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeCapsule(Extent),
		Params);
	bHit2 = GetWorld()->OverlapMultiByChannel(OverlapResult, TraceStart, FQuat::Identity, ECC_Pawn,
	                                          FCollisionShape::MakeCapsule(Extent), Params);
	// ta bort sen
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
		TraceEnd = Origin - Hit.Normal * (Hit.Distance + SkinWidth) * DeltaTime; // delta time? 
		bHit = GetWorld()->SweepSingleByChannel(NormalHit, TraceStart, TraceEnd, FQuat::Identity, ECC_Pawn,
		                                        FCollisionShape::MakeBox(Extent), Params);
		//flytta actor mto normalen av träffpunkten
		SetActorLocation(GetActorLocation() - Hit.Normal * (NormalHit.Distance - SkinWidth) * DeltaTime); // delta time? 
	}
	/*if (!bHit && bHit2)
	{
		FMTDResult MTD;
		bHit = this->GetCapsuleComponent()->ComputePenetration(MTD, FCollisionShape::MakeCapsule(Extent), Origin,
		                                                       FQuat::Identity);
		/*bHit = GetWorld()->SweepSingleByChannel(NormalHit, TraceStart, TraceEnd, FQuat::Identity, ECC_Pawn,
												FCollisionShape::MakeCapsule(Extent), Params);#1#
		//flytta actor mto normalen av träffpunkten
		SetActorLocation(GetActorLocation() +
			MTD.Direction * (MTD.Distance + SkinWidth));
		Velocity += StaticHelperClass::DotProduct(Velocity, -MTD.Direction);
	}*/

	if (Velocity.Size() < 0.1 && Hit.GetActor() != nullptr && Hit.GetActor()->GetVelocity().Size() < 0.1)
	{
		RecursivCounter = 0;
		Velocity = FVector::ZeroVector;
	}
	if (Velocity.Size() < 0.1)
	{
		RecursivCounter = 0;
		Velocity = FVector::ZeroVector;
	}

	if (RecursivCounter > 10)
	{
		RecursivCounter = 0;
		Velocity = FVector::ZeroVector;
	}

	if (bHit || bHit2)
	{
		if (Hit.GetActor() != nullptr)
		{
			// Log the name of the actor that was hit
			UE_LOG(LogTemp, Warning, TEXT("Actor Hit: %s"), *Hit.GetActor()->GetName());
		}

		UE_LOG(LogTemp, Warning, TEXT("Velocity Before: X=%f, Y=%f, Z=%f"), Velocity.X, Velocity.Y, Velocity.Z);

		FVector NormalPower = StaticHelperClass::DotProduct(Velocity, Hit.ImpactNormal);

		Velocity += NormalPower;

		// Log the normal power
		UE_LOG(LogTemp, Warning, TEXT("Normal Power: X=%f, Y=%f, Z=%f"), NormalPower.X, NormalPower.Y, NormalPower.Z);


		/*FVector GroundMovement = Hit.GetActor()->GetVelocity();
		if (GroundMovement.Size() > 0.1)
		{
			UE_LOG(LogTemp, Warning, TEXT("Ground Movement size = %f"), GroundMovement.Size());
			FVector Difference = FVector::ZeroVector;
			Difference.X = Velocity.X - GroundMovement.X;
			FVector FriktionPower = NormalPower * StaticFrictionCoefficient;
			if (Difference.Size() > FriktionPower.Size())
			{
				Velocity.X -= Difference.X;
			}
		}*/
		RecursivCounter++;
		ApplyFriction(DeltaTime, NormalPower.Size());
		UpdateVelocity(DeltaTime);
	}
	UE_LOG(LogTemp, Warning, TEXT("RecursivCounter: %d"), RecursivCounter);

	RecursivCounter = 0;
}

void APlayerCharThreeD::CalculateInput(float DeltaTime)
{
	//normalisera input vektor innan vi * med accelaration 
	if (CurrentInput.Size() > 1) CurrentInput.Normalize(1);
	Velocity += CurrentInput.GetSafeNormal() * Acceleration * DeltaTime;
}

void APlayerCharThreeD::ApplyFriction(float DeltaTime, float NormalMagnitude)
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
