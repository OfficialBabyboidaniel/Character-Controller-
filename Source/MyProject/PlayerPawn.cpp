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
	
	
	DrawDebugBox(GetWorld(), Origin, Extent, FColor::Red, true);
	
	const double Distance = MovementSpeed * DeltaTime;
	const FVector CurrentLocation = GetActorLocation();
	FVector Movement = CurrentInput * Distance;
	//PreventCollision(DeltaTime, Movement);
	
	 SetActorLocation(CurrentLocation + PreventCollision(DeltaTime, Movement));
	if (SweepHit)
	{
		DrawDebugBox(GetWorld(), Hit.ImpactPoint, Extent, FColor::Red, true);
	}
	// kolla här 
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
	
}


FVector APlayerPawn::PreventCollision(float DeltaTime, FVector Movement)
{
	FVector TraceStart = GetActorLocation();
	FVector TraceEnd = Origin + Movement.GetSafeNormal2D() /** (Movement.Size() * SkinWidth)*/;
	FCollisionQueryParams Params;
	GetActorBounds(true, Origin, Extent);
	SweepHit = Sweep(Hit, TraceStart, TraceEnd, Params);
	DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 10.f, 8, FColor::Red, true);
	if (SweepHit) return Movement.GetSafeNormal2D() /** (Hit.Distance - SkinWidth)*/;
	else
		return Movement;
}


bool APlayerPawn::Sweep(FHitResult& HitResult, const FVector& Start, const FVector& Target,
                        FCollisionQueryParams& Params) const
{
	
	Params.AddIgnoredActor(this);
	return GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		Target,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeBox(Extent),
		Params);
}
