// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPawn.h"

#include <Windows.Graphics.Display.h>

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Sampling/NormalHistogram.h"
#include "StaticHelperClass.h"
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

	/*
	GetActorBounds(true, Origin, Extent);
	const double Distance = MovementSpeed * DeltaTime;
	const FVector Movement = CurrentInput * Distance * DeltaTime;
	const FVector GravityForce = FVector::DownVector * Gravity * DeltaTime;
	UE_LOG(LogTemp, Display, TEXT("Gravity vector = %s"), *GravityForce.ToString());
	
	Velocity += Movement + GravityForce + JumpMovement;
	
	UE_LOG(LogTemp, Display, TEXT("Velocity: %f"), Velocity.Size());
	PreventCollision(DeltaTime);
	Velocity.Y = 0;
	SetActorLocation(GetActorLocation() + Velocity * DeltaTime);
	JumpMovement = FVector(0);*/

	const double Distance = MovementSpeed * DeltaTime;
	const FVector CurrentLocation = GetActorLocation();
	FVector Movement = CurrentInput * Distance;
	PreventCollision(DeltaTime, Movement);
	if (!SweepHit) SetActorLocation(CurrentLocation + Movement);
	// kolla här 
	if(SweepHit) SetActorLocation(CurrentLocation + Movement.GetSafeNormal() * Hit.Distance);
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
	// check if grounded
	// Check what is above
	/*FHitResult GroundedHit;
	FHitResult JumpHit;
	FCollisionQueryParams QueryParams;
	FCollisionQueryParams QueryParams2;
	const FVector TraceEndDownwards = Origin + FVector::DownVector * (SkinWidth + GroundCheckDistance);
	const FVector TraceEndUpwards = Origin + FVector::UpVector * JumpHeight;
	bool CollisionCheck = Sweep(GroundedHit, Origin, TraceEndDownwards, QueryParams);

	if (Sweep(GroundedHit, Origin, TraceEndDownwards, QueryParams))
	{
		if (Sweep(JumpHit, Origin, TraceEndUpwards, QueryParams2))
			JumpMovement = FVector::UpVector * FMath::Min(JumpHeight, TraceEndUpwards.Distance(Origin, TraceEndUpwards));
	
		JumpMovement = FVector::UpVector * JumpHeight;
		UE_LOG(LogTemp, Display, TEXT("Jumpmovment %f"), JumpMovement.Size());
	}
	UE_LOG(LogTemp, Display, TEXT("in jump "));*/
}


void APlayerPawn::PreventCollision(float DeltaTime, FVector Movement)
{
	
	FVector TraceStart = GetActorLocation();
	FVector TraceEnd = GetActorLocation() + Movement;
	FCollisionQueryParams Params;
	SweepHit = Sweep(Hit, TraceStart, TraceEnd, Params);
	UE_LOG(LogTemp, Warning, TEXT("bhit = %hhd"), SweepHit);


	/*FHitResult Hit;
	FVector TraceEnd = Origin + Velocity.GetSafeNormal() * Velocity.Size() * DeltaTime + SkinWidth * Velocity.GetSafeNormal();
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	bool bHit;

	UE_LOG(LogTemp, Display, TEXT("in PreventCollision"));

	bHit = Sweep(Hit, Origin, TraceEnd, QueryParams);

	if (Velocity.Size() * DeltaTime <  SmallMovement)
	{
		UE_LOG(LogTemp, Warning, TEXT("returning zero vector, small movement"));
		Velocity = FVector::ZeroVector;
	}
	else if (recursiveCounter == 10)
	{
		UE_LOG(LogTemp, Warning, TEXT("Recursive LOOP: %f"), recursiveCounter);
		recursiveCounter = 0;
		Velocity = FVector::ZeroVector;
	}
	else if (bHit)
	{
		UE_LOG(LogTemp, Display, TEXT("Collision detected 1"));

		FVector TraceStart = Hit.ImpactPoint - Hit.Normal * Hit.Distance;
		TraceStart.Y -= Extent.Y;
		FVector NormalTraceEnd = Hit.ImpactPoint;
		NormalTraceEnd.Y -= Extent.Y;
		FHitResult NormalHit;
		FCollisionQueryParams NormalParams;
		NormalParams.AddIgnoredActor(Hit.GetActor());
		// second sweep
		bHit = Sweep(NormalHit, TraceStart, NormalTraceEnd, NormalParams);
		if (bHit)
		{

			Velocity = -(NormalHit.Normal * (NormalHit.Distance + SkinWidth));
			UE_LOG(LogTemp, Display, TEXT("Return 2, %d"), recursiveCounter++);
			return;
		}

		FVector NormalF = StaticHelperClass::DotProduct(Velocity, Hit.ImpactNormal);
		Velocity += NormalF + Hit.ImpactNormal * SkinWidth;
		UE_LOG(LogTemp, Display, TEXT("Finished, %d"), recursiveCounter++);
		UE_LOG(LogTemp, Display, TEXT("looking for more collisions"));
		Velocity = NormalF;
	}
	*/
}


bool APlayerPawn::Sweep(FHitResult& HitResult, const FVector& Start, const FVector& Target,
                        FCollisionQueryParams& Params) const
{
	Params.AddIgnoredActor(this);
	FVector Origin, Extent;
	GetActorBounds(true, Origin, Extent);
	return GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		Target,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeBox(Extent),
		Params);
}
