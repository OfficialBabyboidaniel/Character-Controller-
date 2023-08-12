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
#include "Quaternion.h"
#include "StateMachineComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h" // Include the SpringArmComponent header
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Math/UnrealMathUtility.h"
//#include "Camera/Component.h"

// Sets default values
APlayerCharThreeD::APlayerCharThreeD()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	StateMachineComponent = CreateDefaultSubobject<UStateMachineComponent>(TEXT("StateMachine"));
}

// Called when the game starts or when spawned
void APlayerCharThreeD::BeginPlay()
{
	Super::BeginPlay();
	Camera = FindComponentByClass<UCameraComponent>();
	if (Camera)
	{
		CameraLocationRelativeToPlayer = Camera->GetComponentLocation();
		// get distance between player and camera
		OffsetDistance = FVector::Distance(GetActorLocation(), CameraLocationRelativeToPlayer);
	}
	Params.AddIgnoredActor(this);

	if(StateMachineComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("StateMachine created correctly"));
	}
}


// Called every frame
void APlayerCharThreeD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//camera rotaion
	CameraRotation = FQuat::MakeFromEuler(CameraInput);

	if (Camera)
	{
		SetInitialCameraLocation(DeltaTime);
		CameraCollisionCheck();
	}

	Gravity = FVector::DownVector * GravityForce * DeltaTime;
	//UE_LOG(LogTemp, Warning, TEXT("Gravity vector: X=%f, Y=%f, Z=%f"), Gravity.X, Gravity.Y, Gravity.Z);
	//add jump and gravity 
	Velocity += Gravity + JumpMovement;
	//UE_LOG(LogTemp, Warning, TEXT("Velocity with added gravity %s"), *Velocity.ToString()); // Log Velocity
	//calculate input.
	CalculateInput(DeltaTime);
	//multiplicera med Air resistance
	//UE_LOG(LogTemp, Warning, TEXT("Velocity with with added input %s"), *Velocity.ToString()); // Log Velocity

	Velocity *= FMath::Pow(AirResistanceCoefficient, DeltaTime);

	// UE_LOG(LogTemp, Warning, TEXT("Velocity after air resistance input %s"), *Velocity.ToString()); // Log Velocity

	double StartTime = FPlatformTime::Seconds();

	// Call the UpdateVelocity function+
	UpdateVelocity(DeltaTime);
	//UE_LOG(LogTemp, Warning, TEXT("Velocity after collision function %s"), *Velocity.ToString()); // Log Velocity
	// Get the current time after UpdateVelocity has finished
	double EndTime = FPlatformTime::Seconds();

	// Calculate the time taken by UpdateVelocity
	double TimeTaken = EndTime - StartTime;

	// Subtract the time taken by UpdateVelocity from DeltaTime
	float AdjustedDeltaTime = FMath::Max(DeltaTime - TimeTaken, 0.0f);

	if (Velocity.Size() > MaxSpeed)
	{
		Velocity = Velocity.GetClampedToMaxSize(MaxSpeed);
	}
	//UE_LOG(LogTemp, Warning, TEXT("Velocity without delta time : %s"), *Velocity.ToString()); // Log Velocity
	SetActorLocation(GetActorLocation() + Velocity * AdjustedDeltaTime);
	//UE_LOG(LogTemp, Warning, TEXT("Velocity whit delta time %s"), *(Velocity * AdjustedDeltaTime).ToString());
	// Log Velocity
	JumpMovement = FVector::ZeroVector;
	CurrentInput = FVector::ZeroVector;
	// måste kollas igenom 
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
	CurrentInput = FVector(AxisValue, CurrentInput.Y, CurrentInput.X);
}


// vertical axis input
void APlayerCharThreeD::YInput(float AxisValue)
{
	CurrentInput = FVector(CurrentInput.X, AxisValue, CurrentInput.Z);
}


void APlayerCharThreeD::LookRight(float AxisValue)
{
	YawAxisValue = AxisValue;
	CameraInput.Z += YawAxisValue * MouseSensitivity;
}

void APlayerCharThreeD::LookUp(float AxisValue)
{
	PitchAxisValue = AxisValue;
	CameraInput.Y += PitchAxisValue * MouseSensitivity;
	CalculatePitchInput();
}


// jump input
void APlayerCharThreeD::JumpInput()
{
	GetActorBounds(true, Origin, Extent);

	FHitResult Hit;
	FVector TraceEnd = Origin + FVector::DownVector * (GroundCheckDistance + SkinWidth);

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
	UE_LOG(LogTemp, Warning, TEXT("TraceEnd vector: X=%f, Y=%f, Z=%f"), TraceEnd.X, TraceEnd.Y, TraceEnd.Z);
	UE_LOG(LogTemp, Warning, TEXT("TraceEnd vector size: %f"), TraceEnd.Size());
	TArray<FOverlapResult> OverlapResult;


	bool bHit = false;
	bool bHit2 = false;
	bHit = GetWorld()->SweepSingleByChannel(
		Hit,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeCapsule(Extent),
		Params);
	/*bHit2 = GetWorld()->OverlapMultiByChannel(OverlapResult, TraceStart, FQuat::Identity, ECC_Pawn,
	                                          FCollisionShape::MakeCapsule(Extent), Params);*/

	UE_LOG(LogTemp, Warning, TEXT("Velocity after first sweep %s"), *Velocity.ToString());
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
		UE_LOG(LogTemp, Warning, TEXT("First bhit true"));
		TraceEnd = Origin - Hit.Normal * (Hit.Distance + SkinWidth) /** DeltaTime*/; // delta time?
		UE_LOG(LogTemp, Warning, TEXT("TraceEnd: %s"), *TraceEnd.ToString());
		bHit = GetWorld()->SweepSingleByChannel(NormalHit, TraceStart, TraceEnd, FQuat::Identity, ECC_Pawn,
		                                        FCollisionShape::MakeCapsule(Extent), Params);
		DrawDebugLine(
			GetWorld(),
			TraceStart,
			TraceEnd,
			FColor::Blue, // Line color (you can change this to any color you like)
			false, // Persistent (false means the line will disappear after one frame)
			5.0f, // LifeTime (negative value means the line will stay forever)
			0, // DepthPriority (you can adjust this if necessary)
			2.0f // Thickness (you can adjust this to change the line's thickness)
		);
		UE_LOG(LogTemp, Warning, TEXT("Velocity after Second sweep %s"), *Velocity.ToString());

		//flytta actor mto normalen av träffpunkten
		UE_LOG(LogTemp, Warning, TEXT("Actor Location before: %s"), *GetActorLocation().ToString());
		SetActorLocation(GetActorLocation() - Hit.Normal * (NormalHit.Distance - SkinWidth) * DeltaTime);
		UE_LOG(LogTemp, Warning, TEXT("Actor Location after: %s"), *GetActorLocation().ToString());
		// delta time? 
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
		UE_LOG(LogTemp, Warning, TEXT("Velocity after normal power sweep %s"), *Velocity.ToString());
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
	FQuat InputQuat = FQuat::MakeFromEuler(EulerRotation);

	if (CurrentInput.Size() > 0.1)
		CurrentInput = InputQuat * CurrentInput.GetSafeNormal();

	GetActorBounds(true, Origin, Extent);
	FHitResult Hit;
	FVector TraceEnd = Origin + FVector::DownVector * (GroundCheckDistance + SkinWidth);

	bool bHit = GetWorld()->SweepSingleByChannel(Hit, Origin, TraceEnd, FQuat::Identity, ECC_Pawn,
	                                             FCollisionShape::MakeCapsule(Extent), Params);
	if (bHit)
	{
		CurrentInput = FVector::VectorPlaneProject(CurrentInput.GetSafeNormal() * Acceleration * DeltaTime,
		                                           Hit.ImpactNormal);
	}
	if (CurrentInput.Size() > 1) CurrentInput.Normalize(1);
	//UE_LOG(LogTemp, Warning, TEXT("CurrentInput size: %f"), CurrentInput.Size());
	Velocity += (CurrentInput.GetSafeNormal() * Acceleration * DeltaTime);
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

//helper functions

//Camera
void APlayerCharThreeD::CalculatePitchInput()
{
	CameraRotation = FQuat::MakeFromEuler(CameraInput);
	EulerRotation = CameraRotation.Euler();
	//clamp pitch value if reached max/min rotaion
	if (EulerRotation.Y > MaxPitchRotaion || EulerRotation.Y < MinPitchRotation)
	{
		EulerRotation.Y = FMath::Clamp(EulerRotation.Y, MinPitchRotation, MaxPitchRotaion);
		//remove value so it does not add to the input forever, prevents a 180* spin 
		CameraInput.Y -= PitchAxisValue * MouseSensitivity;
		CameraRotation.Y = EulerRotation.Y;
	}
}

void APlayerCharThreeD::SetInitialCameraLocation(float DeltaTime)
{
	CameraRotation *= DeltaTime;
	Camera->SetRelativeRotation(CameraRotation);
	FVector OffsetDirection = -Camera->GetRelativeRotation().Quaternion().GetForwardVector();
	OffsetDirection *= OffsetDistance;
	Camera->SetRelativeLocation(OffsetDirection);
}

void APlayerCharThreeD::CameraCollisionCheck()
{
	FHitResult CameraHit;
	GetActorBounds(true, Origin, Extent);
	FVector TraceStart = Origin;
	FVector TraceEnd = Camera->GetComponentLocation();
	UE_LOG(LogTemp, Warning, TEXT("TraceEnd Size: %f"), TraceEnd.Size());
	UE_LOG(LogTemp, Warning, TEXT("OffsetDirection Size: %f"), OffsetDistance);
	bool CameraSweep = GetWorld()->SweepSingleByChannel(CameraHit, TraceStart, TraceEnd, FQuat::Identity, ECC_Pawn,
														FCollisionShape::MakeSphere(CameraSkinWidth * 2), Params);
	if (CameraSweep)
	{
		FVector NewOffsetDirection = -Camera->GetRelativeRotation().Quaternion().GetForwardVector();
		Camera->SetRelativeLocation(NewOffsetDirection *= (CameraHit.Distance - CameraSkinWidth));
		FColor TraceColor = CameraHit.bBlockingHit ? FColor::Red : FColor::Green;
		FVector HitLocation = CameraHit.ImpactPoint;
		DrawDebugSphere(GetWorld(), HitLocation, 10, 12, TraceColor, false, 1.0f);
	}
}
