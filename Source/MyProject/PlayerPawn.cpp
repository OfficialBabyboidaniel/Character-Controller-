// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPawn.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Sampling/NormalHistogram.h"
#include "StaticHelperClass.h"
// Sets default values
APlayerPawn::APlayerPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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

	PlayerCamera->SetupAttachment(RootComponent);
	PlayerCamera->SetRelativeLocation(FVector(0.0f, 1600.0f, 200.0f));
	PlayerCamera->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

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
	// Handle movement based on our "MoveX" and "MoveY" axes
	const double Distance = MovementSpeed * DeltaTime;

	FVector GravityForce = FVector::DownVector * Gravity * DeltaTime;

	UE_LOG(LogTemp, Display, TEXT("curreninput is not zero"));
	FVector Direction = CurrentInput;
	Velocity = Direction * Distance + GravityForce + (bJump ? FVector(0.0f, 0.0f, (JumpHeight)) : FVector::Zero());
	UE_LOG(LogTemp, Display, TEXT("Velocity: %f"), Velocity.Size());
	PreventCollision();
	FVector CurrentLocation = GetActorLocation();
	SetActorLocation(CurrentLocation  + Velocity);
	bJump = false;
}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerPawn::JumpInput);

	PlayerInputComponent->BindAxis(
		"Vertical",
		this,
		&APlayerPawn::VerticalInput);

	PlayerInputComponent->BindAxis(
		"Horizontal",
		this,
		&APlayerPawn::HorizontalInput);
}

void APlayerPawn::HorizontalInput(float AxisValue)
{

	CurrentInput = FVector(AxisValue, 0.0f, CurrentInput.Z);
}

void APlayerPawn::VerticalInput(float AxisValue)
{

	CurrentInput = FVector(CurrentInput.X, 0.0f, AxisValue);
}

void APlayerPawn::JumpInput()
{
	FHitResult Hit;
	// get actor middle point and size
	FVector Origin, Extent;
	GetActorBounds(true, Origin, Extent);

	const FVector TraceEnd = Origin + FVector::DownVector * (GroundCheckDistance + SkinWidth);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	UE_LOG(LogTemp, Display, TEXT("in PreventCollision"));
	bool bHit = GetWorld()->SweepSingleByChannel(Hit, Origin, TraceEnd, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeBox(Extent), QueryParams);
	if (bHit)
		bJump = true;
}

void APlayerPawn::PreventCollision()
{	

	FHitResult Hit;
	// get actor middle point and size
	//first Sweep
	FVector Origin, Extent;
	GetActorBounds(true, Origin, Extent);
	const FVector TraceEnd = Origin + Velocity.GetSafeNormal() * (Velocity.Size() + SkinWidth);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	bool bHit;
	UE_LOG(LogTemp, Display, TEXT("in PreventCollision"));
	if (bDrawDebugTraceEndRED)
		DrawDebugBox(GetWorld(), TraceEnd, Extent, FColor::Red, false, DebugLifeTime);
	bHit = GetWorld()->SweepSingleByChannel(Hit, Origin, TraceEnd, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeBox(Extent), QueryParams);
	FHitResult NormalHit;
	if (Velocity.Size() * GetWorld()->GetDeltaSeconds() < 0.1)
	{
		UE_LOG(LogTemp, Display, TEXT("Returning Zero Vector"));
		Velocity =  FVector::ZeroVector;
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
			bool bHitNew;
			//second sweep
			bHitNew = GetWorld()->SweepSingleByChannel(
			NormalHit,
			Origin - Hit.Normal * Hit.Distance /*- SkinWidth*/,
			Hit.ImpactPoint,
			FQuat::Identity,
			ECC_Pawn,
			FCollisionShape::MakeBox(Extent),
			QueryParams);
		//if (!bHitNew)
		//{
		//	UE_LOG(LogTemp, Display, TEXT("no more collisions"));
		//	recursiveCounter = 0;
		//	return -(Hit.Normal * (NormalHit.Distance - SkinWidth));
		//}
		//skälarprodukten

		FVector DotProduct = StaticHelperClass::DotProduct(Velocity, Hit.ImpactNormal);
		FVector NormalF = Velocity.GetSafeNormal();
		FVector NormalizedMovement = DotProduct + NormalF;
		recursiveCounter++;
		UE_LOG(LogTemp, Display, TEXT("looking for more collisions"));
		Velocity = NormalizedMovement;
		PreventCollision();
	}
	else
	{
		recursiveCounter = 0;
		UE_LOG(LogTemp, Display, TEXT("returning movement"));
	//	SetActorLocation(GetActorLocation() - Hit.Normal * (NormalHit.Distance - SkinWidth));
	}
}
