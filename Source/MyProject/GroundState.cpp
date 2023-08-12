// Fill out your copyright notice in the Description page of Project Settings.


#include "GroundState.h"

#include "PlayerCharThreeD.h"
#include "StaticHelperClass.h"
#include "Camera/CameraComponent.h"

// are all of these needed?
#include <Windows.Graphics.Display.h>
#include "Styling/StyleColors.h"
#include "DrawDebugHelpers.h"
#include "Quaternion.h"
#include "StateMachineComponent.h"
#include "GameFramework/SpringArmComponent.h" // Include the SpringArmComponent header
#include "Math/UnrealMathUtility.h"

UGroundState::UGroundState()
{
}

void UGroundState::BeginPlay()
{
	Super::BeginPlay();
	Camera = PlayerCharThreeD->FindComponentByClass<UCameraComponent>();
	if (Camera)
	{
		CameraLocationRelativeToPlayer = Camera->GetComponentLocation();
		// get distance between player and camera
		OffsetDistance = FVector::Distance(PlayerCharThreeD->GetActorLocation(), CameraLocationRelativeToPlayer);
	}

	Params.AddIgnoredActor(PlayerCharThreeD);
}

void UGroundState::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bShouldTick)
	{
		Update(DeltaTime);
	}
}

void UGroundState::Update(float DeltaTime)
{
	Super::Update(DeltaTime);
	//UE_LOG(LogTemp, Warning, TEXT("groundState ticking"));

	//camera rotaion
	CameraInput.Z += PlayerCharThreeD->GetYawAxisValue() * MouseSensitivity;
	CameraInput.Y += PlayerCharThreeD->GetPitchAxisValue() * MouseSensitivity;
	CalculatePitchInput();
	CameraRotation = FQuat::MakeFromEuler(CameraInput);
	
	if (Camera)
	{
		SetInitialCameraLocation(DeltaTime);
		CameraCollisionCheck();
	}

	CaluclateInitialVelocity(DeltaTime);
	CalculateInput(DeltaTime);


	//multiplicera med Air resistance
	//UE_LOG(LogTemp, Warning, TEXT("Velocity with with added input %s"), *Velocity.ToString()); // Log Velocity
	Velocity *= FMath::Pow(AirResistanceCoefficient, DeltaTime);

	// UE_LOG(LogTemp, Warning, TEXT("Velocity after air resistance input %s"), *Velocity.ToString()); // Log Velocity
	double StartTime = FPlatformTime::Seconds();
	// Call the UpdateVelocity function+
	//
	UpdateVelocity(DeltaTime);
	//
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
	PlayerCharThreeD->SetActorLocation(PlayerCharThreeD->GetActorLocation() + Velocity * AdjustedDeltaTime);
	//SetActorLocation(GetActorLocation() + Velocity * AdjustedDeltaTime);
	//UE_LOG(LogTemp, Warning, TEXT("Velocity whit delta time %s"), *(Velocity * AdjustedDeltaTime).ToString());
	// Log Velocity
	PlayerCharThreeD->SetCurrentInput(FVector::ZeroVector);
	PlayerCharThreeD->SetJumpInput(FVector::ZeroVector);

	// måste kollas igenom
}


void UGroundState::CaluclateInitialVelocity(float DeltaTime)
{
	Gravity = FVector::DownVector * GravityForce * DeltaTime;

	Velocity += Gravity + PlayerCharThreeD->GetJumpInput();
}

void UGroundState::UpdateVelocity(float DeltaTime)
{
	PlayerCharThreeD->GetActorBounds(true, Origin, Extent);
	FHitResult Hit;
	FHitResult NormalHit;
	FVector TraceStart = Origin;
	FVector TraceEnd = Origin + Velocity.GetSafeNormal() * (Velocity.Size() + SkinWidth) * DeltaTime;
	//UE_LOG(LogTemp, Warning, TEXT("TraceEnd vector: X=%f, Y=%f, Z=%f"), TraceEnd.X, TraceEnd.Y, TraceEnd.Z);
	//UE_LOG(LogTemp, Warning, TEXT("TraceEnd vector size: %f"), TraceEnd.Size());
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

	//	UE_LOG(LogTemp, Warning, TEXT("Velocity after first sweep %s"), *Velocity.ToString());
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
		//UE_LOG(LogTemp, Warning, TEXT("First bhit true"));
		TraceEnd = Origin - Hit.Normal * (Hit.Distance + SkinWidth) /** DeltaTime*/; // delta time?
		//UE_LOG(LogTemp, Warning, TEXT("TraceEnd: %s"), *TraceEnd.ToString());
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
		//UE_LOG(LogTemp, Warning, TEXT("Velocity after Second sweep %s"), *Velocity.ToString());

		//flytta actor mto normalen av träffpunkten
		//UE_LOG(LogTemp, Warning, TEXT("Actor Location before: %s"), *GetActorLocation().ToString());
		PlayerCharThreeD->SetActorLocation(
			PlayerCharThreeD->GetActorLocation() - Hit.Normal * (NormalHit.Distance - SkinWidth) * DeltaTime);
		//UE_LOG(LogTemp, Warning, TEXT("Actor Location after: %s"), *GetActorLocation().ToString());
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
			//UE_LOG(LogTemp, Warning, TEXT("Actor Hit: %s"), *Hit.GetActor()->GetName());
		}

		//UE_LOG(LogTemp, Warning, TEXT("Velocity Before: X=%f, Y=%f, Z=%f"), Velocity.X, Velocity.Y, Velocity.Z);

		FVector NormalPower = StaticHelperClass::DotProduct(Velocity, Hit.ImpactNormal);

		Velocity += NormalPower;
		//UE_LOG(LogTemp, Warning, TEXT("Velocity after normal power sweep %s"), *Velocity.ToString());
		// Log the normal power
		//UE_LOG(LogTemp, Warning, TEXT("Normal Power: X=%f, Y=%f, Z=%f"), NormalPower.X, NormalPower.Y, NormalPower.Z);


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
	//UE_LOG(LogTemp, Warning, TEXT("RecursivCounter: %d"), RecursivCounter);

	RecursivCounter = 0;
}

void UGroundState::CalculateInput(float DeltaTime)
{
	FQuat InputQuat = FQuat::MakeFromEuler(EulerRotation);

	if (PlayerCharThreeD->GetCurrentInput().Size() > 0.1)
	{
		PlayerCharThreeD->GetCurrentInput() = InputQuat * PlayerCharThreeD->GetCurrentInput().GetSafeNormal();
		UE_LOG(LogTemp, Warning, TEXT("CurrentInput size: %f"), PlayerCharThreeD->GetCurrentInput().Size());
	}


	PlayerCharThreeD->GetActorBounds(true, Origin, Extent);
	FHitResult Hit;
	FVector TraceEnd = Origin + FVector::DownVector * (GroundCheckDistance + SkinWidth);

	bool bHit = GetWorld()->SweepSingleByChannel(Hit, Origin, TraceEnd, FQuat::Identity, ECC_Pawn,
	                                             FCollisionShape::MakeCapsule(Extent), Params);
	if (bHit)
	{
		PlayerCharThreeD->GetCurrentInput() = FVector::VectorPlaneProject(
			PlayerCharThreeD->GetCurrentInput().GetSafeNormal() * Acceleration * DeltaTime,
			Hit.ImpactNormal);
	}
	if (PlayerCharThreeD->GetCurrentInput().Size() > 1) PlayerCharThreeD->GetCurrentInput().Normalize(1);
	//UE_LOG(LogTemp, Warning, TEXT("CurrentInput size: %f"), CurrentInput.Size());
	Velocity += (PlayerCharThreeD->GetCurrentInput().GetSafeNormal() * Acceleration * DeltaTime);
}

void UGroundState::ApplyFriction(float DeltaTime, float NormalMagnitude)
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
void UGroundState::CalculatePitchInput()
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

void UGroundState::SetInitialCameraLocation(float DeltaTime)
{
	CameraRotation *= DeltaTime;
	Camera->SetRelativeRotation(CameraRotation);
	FVector OffsetDirection = -Camera->GetRelativeRotation().Quaternion().GetForwardVector();
	OffsetDirection *= OffsetDistance;
	Camera->SetRelativeLocation(OffsetDirection);
}

void UGroundState::CameraCollisionCheck()
{
	FHitResult CameraHit;
	PlayerCharThreeD->GetActorBounds(true, Origin, Extent);
	FVector TraceStart = Origin;
	FVector TraceEnd = Camera->GetComponentLocation();
	//UE_LOG(LogTemp, Warning, TEXT("TraceEnd Size: %f"), TraceEnd.Size());
	//UE_LOG(LogTemp, Warning, TEXT("OffsetDirection Size: %f"), OffsetDistance);
	bool CameraSweep = GetWorld()->SweepSingleByChannel(CameraHit, TraceStart, TraceEnd, FQuat::Identity, ECC_Pawn,
	                                                    FCollisionShape::MakeSphere(CameraSkinWidth * 2), Params);
	if (CameraSweep)
	{
		FVector NewOffsetDirection = -Camera->GetRelativeRotation().Quaternion().
		                                                GetForwardVector();
		Camera->SetRelativeLocation(
			NewOffsetDirection *= (CameraHit.Distance - CameraSkinWidth));
		FColor TraceColor = CameraHit.bBlockingHit ? FColor::Red : FColor::Green;
		FVector HitLocation = CameraHit.ImpactPoint;
		DrawDebugSphere(GetWorld(), HitLocation, 10, 12, TraceColor, false, 1.0f);
	}
}
