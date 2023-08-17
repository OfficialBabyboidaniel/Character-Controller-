// Fill out your copyright notice in the Description page of Project Settings.


#include "AirState.h"

#include "PlayerCharThreeD.h"
#include "StateMachineComponent.h"
#include "StaticHelperClass.h"
#include "Components/CapsuleComponent.h"

UAirState::UAirState()
{
}

void UAirState::BeginPlay()
{
	Super::BeginPlay();

	Params.AddIgnoredActor(PlayerCharThreeD);
}

void UAirState::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bShouldTick)
	{
		Update(DeltaTime);
	}
}

void UAirState::Update(float DeltaTime)
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

	PlayerCharThreeD->GetActorBounds(true, Origin, Extent);
	FHitResult Hit;
	FVector TraceEnd = Origin + FVector::DownVector * (GroundCheckDistance + SkinWidth);
	bool bHit = GetWorld()->SweepSingleByChannel(Hit, Origin, TraceEnd, FQuat::Identity, ECC_Pawn,
	                                             FCollisionShape::MakeCapsule(Extent), Params);
	if (bHit)
	{
		PlayerCharThreeD->SetVelocity(Velocity);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("Changing state to ground state"));
		//change to  groundstate
		PlayerCharThreeD->GetStateMachine()->ChangeState(PlayerCharThreeD->GetStateMachine()->States[0]);
	}
	//check done, continue with update function
}


void UAirState::UpdateVelocity(float DeltaTime)
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
		TraceEnd = Origin - Hit.Normal * (Hit.Distance + SkinWidth) /** DeltaTime*/; // delta time?
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

	RecursivCounter = 0;
}

void UAirState::OverlapCollisionUpdate(float DeltaTime)
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

			bool bCanResolveCollision = PlayerCharThreeD->GetCapsuleComponent()->ComputePenetration(
				MTD, OverlapResult[0].GetComponent()->GetCollisionShape(), ColliderOrigin,
				OverlapResult[0].GetActor()->GetActorQuat()
			);

			//moving the collision way out. im litterally just teleporting

			if (bCanResolveCollision)
			{
				// sätt en begränsing på förflyttningens storlek
				if (FVector::Distance(OriginalLocationBeforeUpdate, PlayerCharThreeD->GetActorLocation() +
				                      MTD.Direction * (MTD.Distance + SkinWidth)) < SkinWidth * 2)
				{
					PlayerCharThreeD->SetActorLocation(PlayerCharThreeD->GetActorLocation() +
						MTD.Direction * (MTD.Distance + SkinWidth) * DeltaTime);
				}
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
	ApplyFriction(DeltaTime, StaticHelperClass::DotProduct(Velocity, -MTD.Direction).Size());
	RecursivCounter = 0;
}

void UAirState::ApplyFriction(float DeltaTime, float NormalMagnitude)
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

//velocity 
void UAirState::CalculateInitialVelocity(float DeltaTime)
{
	Gravity = FVector::DownVector * GravityForce * DeltaTime;

	Velocity += Gravity + PlayerCharThreeD->GetJumpInput();
}

//input
void UAirState::CalculateInput(float DeltaTime)
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
