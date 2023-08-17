// Fill out your copyright notice in the Description page of Project Settings.


#include "GroundState.h"
#include "PlayerCharThreeD.h"
#include "StaticHelperClass.h"
#include "Camera/CameraComponent.h"
// are all of these needed?
#include "Styling/StyleColors.h"
#include "DrawDebugHelpers.h"
#include "Quaternion.h"
#include "StateMachineComponent.h"
#include "Components/CapsuleComponent.h"
#include "Math/UnrealMathUtility.h"

UGroundState::UGroundState()
{
}

void UGroundState::BeginPlay()
{
	Super::BeginPlay();
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

	CurrentInput = PlayerCharThreeD->GetCurrentInput();
	CalculateInitialVelocity(DeltaTime);
	CalculateInput(DeltaTime);

	Velocity *= FMath::Pow(AirResistanceCoefficient, DeltaTime);

	const double StartTime = FPlatformTime::Seconds();
	OriginalLocationBeforeUpdate = PlayerCharThreeD->GetActorLocation();
	UpdateVelocity(DeltaTime);
	const double EndTime = FPlatformTime::Seconds();
	const double TimeTaken = EndTime - StartTime;
	const float AdjustedDeltaTime = FMath::Max(DeltaTime - TimeTaken, 0.0f);

	if (Velocity.Size() > MaxSpeed)
	{
		Velocity = Velocity.GetClampedToMaxSize(MaxSpeed);
	}

	PlayerCharThreeD->SetActorLocation(PlayerCharThreeD->GetActorLocation() + Velocity * AdjustedDeltaTime);
	//reset values for next tick calculation
	PlayerCharThreeD->SetCurrentInput(FVector::ZeroVector);
	PlayerCharThreeD->SetJumpInput(FVector::ZeroVector);

	//check if state is valid and if it should change state
	/*PlayerCharThreeD->GetActorBounds(true, Origin, Extent);
	FHitResult Hit;
	const FVector TraceEnd = Origin + FVector::DownVector * (GroundCheckDistance + SkinWidth);
	const bool bHit = GetWorld()->SweepSingleByChannel(Hit, Origin, TraceEnd, FQuat::Identity, ECC_Pawn,
													   FCollisionShape::MakeCapsule(Extent), Params);
	if (!bHit)
	{
		//byte till airstate
		PlayerCharThreeD->GetStateMachine()->ChangeState(PlayerCharThreeD->GetStateMachine()->States[1]);
		return; 
	}*/
	//check done, continue with update function
}


void UGroundState::UpdateVelocity(float DeltaTime)
{
	PlayerCharThreeD->GetActorBounds(true, Origin, Extent);
	Params.AddIgnoredActor(PlayerCharThreeD);
	FHitResult Hit;
	FHitResult NormalHit;
	FVector TraceStart = Origin;
	FVector TraceEnd = Origin + Velocity.GetSafeNormal() * (Velocity.Size() + SkinWidth) * DeltaTime;


	bool bHit = false;
	bHit = GetWorld()->SweepSingleByChannel(
		Hit,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeCapsule(Extent),
		Params);

	if (bHit)
	{
		//UE_LOG(LogTemp, Warning, TEXT("First bhit true"));
		TraceEnd = Origin - Hit.Normal * (Hit.Distance + SkinWidth) /** DeltaTime*/; // delta time?
		//UE_LOG(LogTemp, Warning, TEXT("TraceEnd: %s"), *TraceEnd.ToString());
		bHit = GetWorld()->SweepSingleByChannel(NormalHit, TraceStart, TraceEnd, FQuat::Identity, ECC_Pawn,
		                                        FCollisionShape::MakeCapsule(Extent), Params);
		PlayerCharThreeD->SetActorLocation(
			PlayerCharThreeD->GetActorLocation() - Hit.Normal * (NormalHit.Distance - SkinWidth) * DeltaTime);
	}

	if (Velocity.Size() < 0.1)
	{
		RecursivCounter = 0;
		Velocity = FVector::ZeroVector;
	}

	if (RecursivCounter > 5)
	{
		UE_LOG(LogTemp, Warning, TEXT("recursvie counter normal sweep %d"), RecursivCounter);
		RecursivCounter = 0;
		OverlapCollisionUpdate(DeltaTime);
	}
	else
	{
		if (bHit)
		{
			RecursivCounter++;
			UpdateVelocity(DeltaTime);
		}
	}
	FVector NormalPower = StaticHelperClass::DotProduct(Velocity, Hit.ImpactNormal);
	Velocity += NormalPower;
	ApplyFriction(DeltaTime, NormalPower.Size());
	// Log the normal power

	//UE_LOG(LogTemp, Warning, TEXT("RecursivCounter: %d"), RecursivCounter);

	RecursivCounter = 0;
}

void UGroundState::OverlapCollisionUpdate(float DeltaTime)
{
	PlayerCharThreeD->GetActorBounds(true, Origin, Extent);

	Params.AddIgnoredActor(PlayerCharThreeD);
	const FVector TraceStart = Origin;

	TArray<FOverlapResult> OverlapResult;

	bool bHit2 = false;
	bHit2 = GetWorld()->OverlapMultiByChannel(OverlapResult, TraceStart, FQuat::Identity, ECC_Pawn,
	                                          FCollisionShape::MakeCapsule(Extent), Params);
	FMTDResult MTD;
	if (bHit2)
	{
		if (RecursivCounter < 5)
		{
			FVector ColliderOrigin;
			FVector ColliderExtent;
			OverlapResult[0].GetActor()->GetActorBounds(true, ColliderOrigin, ColliderExtent);

			UE_LOG(LogTemp, Warning, TEXT("Overlapping Actor's Name: %s"), *OverlapResult[0].GetActor()->GetName());

			bool bCanResolveCollision = PlayerCharThreeD->GetCapsuleComponent()->ComputePenetration(
				MTD, OverlapResult[0].GetComponent()->GetCollisionShape(), ColliderOrigin,
				OverlapResult[0].GetActor()->GetActorQuat()
			);

			//moving the collision way out. im litterally just teleporting

			if (bCanResolveCollision)
			{
				UE_LOG(LogTemp, Warning, TEXT("player location before move: %s"),
				       *PlayerCharThreeD->GetActorLocation().ToString());

				// sätt en begränsing på förflyttningens storlek
				if (FVector::Distance(OriginalLocationBeforeUpdate, PlayerCharThreeD->GetActorLocation() +
				                      MTD.Direction * (MTD.Distance + SkinWidth)) < SkinWidth * 2 )
				{
					PlayerCharThreeD->SetActorLocation(PlayerCharThreeD->GetActorLocation() +
						MTD.Direction * (MTD.Distance + SkinWidth) * DeltaTime);
					//delta time? 
				}
				UE_LOG(LogTemp, Warning, TEXT("player location after move: %s"),
				       *PlayerCharThreeD->GetActorLocation().ToString());
				// Recursive call to continue resolving collisions
				RecursivCounter++;
				OverlapCollisionUpdate(DeltaTime);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Reached maximum recursion depth. Exiting collision resolution."));
		}
	}

	Velocity += StaticHelperClass::DotProduct(Velocity, -MTD.Direction);
	UE_LOG(LogTemp, Warning, TEXT("player location after normal power: %s"),
	       *PlayerCharThreeD->GetActorLocation().ToString());
	ApplyFriction(DeltaTime, StaticHelperClass::DotProduct(Velocity, -MTD.Direction).Size());
	RecursivCounter = 0;
}


void UGroundState::CalculateInput(float DeltaTime)
{
	FQuat InputQuat = FQuat::MakeFromEuler(EulerRotation);

	if (CurrentInput.Size() > 0.1)
	{
		CurrentInput = InputQuat * CurrentInput.GetSafeNormal();
	}

	PlayerCharThreeD->GetActorBounds(true, Origin, Extent);
	FHitResult Hit;
	FVector TraceEnd = Origin + FVector::DownVector * (GroundCheckDistance + SkinWidth);

	bool bHit = GetWorld()->SweepSingleByChannel(Hit, Origin, TraceEnd, FQuat::Identity, ECC_Pawn,
	                                             FCollisionShape::MakeCapsule(Extent), Params);
	if (bHit)
	{
		CurrentInput = FVector::VectorPlaneProject(
			CurrentInput.GetSafeNormal() * Acceleration * DeltaTime,
			Hit.ImpactNormal);
	}
	if (CurrentInput.Size() > 1) CurrentInput.Normalize(1);

	Velocity += CurrentInput.GetSafeNormal() * Acceleration * DeltaTime;
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

		//ska delta time vara här? 
	}
}

//helper functions

//velocity 
void UGroundState::CalculateInitialVelocity(float DeltaTime)
{
	Gravity = FVector::DownVector * GravityForce * DeltaTime;

	Velocity += Gravity + PlayerCharThreeD->GetJumpInput();
}
