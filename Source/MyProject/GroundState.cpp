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

		UE_LOG(LogTemp, Warning, TEXT("Velocity Before normal power: X=%f, Y=%f, Z=%f"), Velocity.X, Velocity.Y, Velocity.Z);

		FVector NormalPower = StaticHelperClass::DotProduct(Velocity, Hit.ImpactNormal);

		Velocity += NormalPower;
		UE_LOG(LogTemp, Warning, TEXT("Velocity after normal power sweep %s"), *Velocity.ToString());
		// Log the normal power
		UE_LOG(LogTemp, Warning, TEXT("Normal Power: X=%f, Y=%f, Z=%f"), NormalPower.X, NormalPower.Y, NormalPower.Z);
		
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

