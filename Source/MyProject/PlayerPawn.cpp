// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPawn.h"

#include <Windows.Graphics.Display.h>

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Sampling/NormalHistogram.h"
#include "StaticHelperClass.h"
#include "Styling/StyleColors.h"
#include "Util/ColorConstants.h"
// Sets default values
APlayerPawn::APlayerPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// aktivera tick
	PrimaryActorTick.bCanEverTick = true;
	// auto posses player 0
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	// create root component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	// create mesh
	OurVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OurVisibleComponent"));

	// create camera
	auto PlayerCamera = CreateDefaultSubobject<UCameraComponent>(
		TEXT("PlayerCamera"));

	// setup camera
	PlayerCamera->SetupAttachment(RootComponent);
	PlayerCamera->SetRelativeLocation(FVector(0.0f, 1600.0f, 200.0f));
	PlayerCamera->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	// set to 2d view
	PlayerCamera->ProjectionMode = ECameraProjectionMode::Orthographic;
	PlayerCamera->OrthoWidth = 3200.f;

	OurVisibleComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CalculateInitialVelocity(DeltaTime);
	//calculera input.
	CalculateInput(DeltaTime);

	//multiplicera med luftmotstånd
	Velocity *= FMath::Pow(AirResistanceCoefficient, DeltaTime);
	double StartTime = FPlatformTime::Seconds();
	UpdateVelocity(DeltaTime);
	double EndTime = FPlatformTime::Seconds();

	// Calculate the time taken by UpdateVelocity
	double TimeTaken = EndTime - StartTime;
	float AdjustedDeltaTime = FMath::Max(DeltaTime - TimeTaken, 0.0f);

	// Y axis zero to stay in 2D.
	Velocity.Y = 0;
	//clamp to maxspeed if needed
	if (Velocity.Size() > MaxSpeed)
	{
		Velocity = Velocity.GetClampedToMaxSize(MaxSpeed);
	}
	SetActorLocation(GetActorLocation() + Velocity * AdjustedDeltaTime);
	JumpMovement = FVector::ZeroVector;
}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// bind inputs
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerPawn::JumpInput);
	PlayerInputComponent->BindAxis("Vertical", this, &APlayerPawn::VerticalInput);
	PlayerInputComponent->BindAxis("Horizontal", this, &APlayerPawn::HorizontalInput);
}

// horizontal axis input
void APlayerPawn::HorizontalInput(float AxisValue)
{
	CurrentInput = FVector(AxisValue, 0.0f, CurrentInput.Z);
}

// vertical axis input
void APlayerPawn::VerticalInput(float AxisValue)
{
	CurrentInput = FVector(CurrentInput.X, 0.0f, AxisValue);
}

// jump input
void APlayerPawn::JumpInput()
{
	GetActorBounds(true, Origin, Extent);
	FHitResult Hit;
	FVector TraceEnd = Origin + FVector::DownVector * (GroundCheckDistance + SkinWidth);
	Params.AddIgnoredActor(this);
	bool bHit = GetWorld()->SweepSingleByChannel(Hit, Origin, TraceEnd, FQuat::Identity, ECC_Pawn,
	                                             FCollisionShape::MakeBox(Extent), Params);
	if (bHit)
	{
		JumpMovement = FVector(0, 0, JumpForce);
	}
}

void APlayerPawn::UpdateVelocity(float DeltaTime)
{
	GetActorBounds(true, Origin, Extent);
	FHitResult Hit;
	FHitResult NormalHit;
	FVector TraceStart = Origin;
	FVector TraceEnd = Origin + Velocity.GetSafeNormal() * (Velocity.Size() + SkinWidth) * DeltaTime;
	// ska delta tid vara här? 
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

	if (bHit)
	{
		TraceEnd = Origin - Hit.Normal * (Hit.Distance + SkinWidth);
		bHit = GetWorld()->SweepSingleByChannel(NormalHit, TraceStart, TraceEnd, FQuat::Identity, ECC_Pawn,
		                                        FCollisionShape::MakeBox(Extent), Params);


		
		SetActorLocation(GetActorLocation() - Hit.Normal * (NormalHit.Distance - SkinWidth) * DeltaTime);
		
	}

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
	if (bHit)
	{
		FVector NormalPower = StaticHelperClass::DotProduct(Velocity, Hit.ImpactNormal);
		Velocity += NormalPower;

		FVector GroundMovement = Hit.GetActor()->GetVelocity();
		if (GroundMovement.Size() > 0.1)
		{
			
			UE_LOG(LogTemp, Warning, TEXT("Ground Movement  = %s"), *GroundMovement.ToString());
			UE_LOG(LogTemp, Warning, TEXT("Velocity Movement before = %s"), *Velocity.ToString());
			FVector Difference = FVector::ZeroVector;
			Difference.X = GroundMovement.X - Velocity.X;
			UE_LOG(LogTemp, Warning, TEXT("Difference Movement  = %s"), *Difference.ToString());
			FVector FriktionPower = NormalPower * StaticFrictionCoefficient;
			UE_LOG(LogTemp, Warning, TEXT("Friktion Movement  = %s"), *FriktionPower.ToString());
			Velocity.X += Difference.X;
			UE_LOG(LogTemp, Warning, TEXT("Velocity Movement after = %s"), *Velocity.ToString());
			/*
			if (Difference.Size() < FriktionPower.Size())
			{
				UE_LOG(LogTemp, Warning, TEXT("Velocity Movement before = %s"), *Velocity.ToString());
				Velocity.X = GroundMovement.X;
				UE_LOG(LogTemp, Warning, TEXT("Velocity Movement after  = %s"), *Velocity.ToString());
			}*/
		}

		RecursivCounter++;
		ApplyFriction(DeltaTime, NormalPower.Size());
		UpdateVelocity(DeltaTime);
	}
	RecursivCounter = 0;
}


//Helper functions

void APlayerPawn::CalculateInitialVelocity(float DeltaTime)
{
	Gravity = FVector::DownVector * GravityForce * DeltaTime;

	Velocity += Gravity + JumpMovement;
}

void APlayerPawn::CalculateInput(float DeltaTime)
{
	Velocity += CurrentInput.GetSafeNormal() * Acceleration * DeltaTime;
}

void APlayerPawn::ApplyFriction(float DeltaTime, float NormalMagnitude)
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
