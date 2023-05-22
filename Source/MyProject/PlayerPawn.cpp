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

	//DrawDebugBox(GetWorld(), Origin, Extent, FColor::Red, true);

	const double Distance = MovementSpeed * DeltaTime;
	FVector Gravity = FVector::DownVector * GravityForce * DeltaTime;
	FVector InputMovement = CurrentInput * Distance;
	FVector Velocity = InputMovement + Gravity + JumpMovement;

	const FVector CurrentLocation = GetActorLocation();
	FVector ProcessedMovement = CollisionFunction(Velocity);
	UE_LOG(LogTemp, Warning, TEXT("ProcessedMovement: %s"), *ProcessedMovement.ToString());
	
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
	GetActorBounds(true, Origin, Extent);
	FHitResult Hit;
	FVector TraceStart = Origin;
	FVector TraceEnd = Origin + FVector::DownVector * (GroundCheckDistance + SkinWidth);
	Params.AddIgnoredActor(this);
	bool bHit = GetWorld()->SweepSingleByChannel(Hit, TraceStart, TraceEnd, FQuat::Identity, ECC_Pawn,
	                                             FCollisionShape::MakeBox(Extent), Params);
	if (bHit)
	{
		JumpMovement = FVector(0, 0, JumpForce);
	}
}

FVector APlayerPawn::CollisionFunction(FVector Movement)
{
	GetActorBounds(true, Origin, Extent);
	FHitResult Hit;
	FVector TraceStart = Origin;
	FVector TraceEnd = Origin + Movement.GetSafeNormal() * (Movement.Size() + SkinWidth);
	Params.AddIgnoredActor(this);
	bool bHit = GetWorld()->SweepSingleByChannel(Hit, TraceStart, TraceEnd, FQuat::Identity, ECC_Pawn,
	                                             FCollisionShape::MakeBox(Extent), Params);
	if (bHit)
	{
		FVector NormalF = StaticHelperClass::DotProduct(Movement, Hit.Normal);
		UE_LOG(LogTemp, Warning, TEXT("NormalF: %s"), *NormalF.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Movement: %s"), *Movement.ToString());
		FVector FullMovement = Movement + NormalF;
		UE_LOG(LogTemp, Warning, TEXT("FullMovement: %s"), *FullMovement.ToString());
		return FullMovement;
	}
	return Movement;
}
