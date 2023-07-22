// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPawn.h"

#include <Windows.Graphics.Display.h>

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Sampling/NormalHistogram.h"
#include "StaticHelperClass.h"
#include "Styling/StyleColors.h"
#include "Util/ColorConstants.h"
#include "DrawDebugHelpers.h"
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

	//DrawDebugBox(GetWorld(), Origin, Extent, FColor::Red, true);

	const double Distance = MovementSpeed * DeltaTime;
	FVector Gravity = FVector::DownVector * GravityForce * DeltaTime;
	FVector InputMovement = CurrentInput * Distance;
	Velocity = InputMovement + Gravity + JumpMovement;

	FVector ProcessedMovement = UpdateVelocity(Velocity, RecursivCounter);
	const FVector CurrentLocation = GetActorLocation();
	//	UE_LOG(LogTemp, Warning, TEXT("ProcessedMovement: %s"), *ProcessedMovement.ToString());

	SetActorLocation(CurrentLocation + ProcessedMovement);
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

FVector APlayerPawn::UpdateVelocity(FVector Movement, int counter)
{
	RecursivCounter = counter;
	GetActorBounds(true, Origin, Extent);
	FHitResult Hit;
	FVector TraceStart = Origin;
	FVector TraceEnd = Origin + Movement.GetSafeNormal() * (Movement.Size() + SkinWidth);
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

	if(bHit)
	{
		FHitResult NormalHit;
		TraceEnd = Origin - Hit.Normal * Hit.Distance;
		bHit = GetWorld()->SweepSingleByChannel(NormalHit, TraceStart, TraceEnd, FQuat::Identity, ECC_Pawn,
		                                        FCollisionShape::MakeBox(Extent), Params);
		// should you set actor location on collision hit, prolly not? 
		SetActorLocation(GetActorLocation() - Hit.Normal * (NormalHit.Distance - SkinWidth));
	}
	
	if (Movement.Size() < 0.1)
	{
		//	UE_LOG(LogTemp, Warning, TEXT("Movement too small, returning zero vector."));
		RecursivCounter = 0;
		return FVector::ZeroVector;
	}

	if (RecursivCounter > 10)
	{
		UE_LOG(LogTemp, Warning, TEXT("Processedmomvenet"));
		RecursivCounter = 0;
		return FVector::ZeroVector;
	}
	UE_LOG(LogTemp, Warning, TEXT("Counter %d"), RecursivCounter);
	if (bHit)
	{
		//		UE_LOG(LogTemp, Warning, TEXT("Collision Movement %s"), *Movement.ToString());

		//		UE_LOG(LogTemp, Warning, TEXT("Processedmomvenet %s"), *test.ToString());

		//behöver recursiv function parametrar öndras ifall vi gör två boxcasts och flyttar mot normalen av första träffpunkt
		return UpdateVelocity(StaticHelperClass::DotProduct(Movement, Hit.ImpactNormal) +
		                         Movement + Movement.GetSafeNormal() *
		                         (Hit.Distance - SkinWidth), ++RecursivCounter);
	}
	RecursivCounter = 0;
	UE_LOG(LogTemp, Warning, TEXT("Final Movement %s"), *Movement.ToString());
	return Movement;
}
