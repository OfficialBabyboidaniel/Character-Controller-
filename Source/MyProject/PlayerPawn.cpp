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

	GetActorBounds(true, Origin, Extent);
	const double Distance = MovementSpeed * DeltaTime;
	const FVector Movement = CurrentInput * Distance;
	const FVector GravityForce = FVector::DownVector * Gravity * DeltaTime;
	Velocity += Movement + GravityForce + JumpMovement;

	UE_LOG(LogTemp, Display, TEXT("Ticking"));
	UE_LOG(LogTemp, Display, TEXT("Velocity: %f"), Velocity.Size());
	PreventCollision(DeltaTime);
	Velocity.Y = 0;
	SetActorLocation(GetActorLocation() + Velocity * DeltaTime);
	bJump = false;
	JumpMovement = FVector(0);
}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerPawn::JumpInput);
	PlayerInputComponent->BindAxis("Vertical", this, &APlayerPawn::VerticalInput);
	PlayerInputComponent->BindAxis("Horizontal", this, &APlayerPawn::HorizontalInput);
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
	FHitResult Hit2;
	FCollisionQueryParams QueryParams;
	FCollisionQueryParams QueryParams2;
	// get actor middle point and size
	const FVector TraceEnd1 = Origin + FVector::DownVector * (SkinWidth + GroundCheckDistance);
	const FVector TraceEnd2 = Origin + FVector::UpVector * JumpHeight;
	bool CollisionCheck = Sweep(Hit, Origin, TraceEnd1, QueryParams);

	if(Sweep(Hit, Origin, TraceEnd1, QueryParams)){
		if(Sweep(Hit2, Origin, TraceEnd2, QueryParams2))
		JumpMovement = FVector::UpVector * FMath::Min(JumpHeight, TraceEnd2.Distance(Origin, TraceEnd2));
		bJump = true;

	} else {
		JumpMovement = FVector::UpVector * JumpHeight;
	}
	UE_LOG(LogTemp, Display, TEXT("in jump "));
}

bool APlayerPawn::Sweep(FHitResult &HitResult, const FVector &Start, const FVector &Target, FCollisionQueryParams &Params) const
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

void APlayerPawn::PreventCollision(float DeltaTime)
{
	FHitResult Hit;
	FVector TraceEnd = Origin + Velocity.GetSafeNormal() * Velocity.Size() * DeltaTime + SkinWidth * Velocity.GetSafeNormal();
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	bool bHit;

	UE_LOG(LogTemp, Display, TEXT("in PreventCollision"));

	bHit = Sweep(Hit, Origin, TraceEnd, QueryParams);

	if (Velocity.Size() < 0.1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Returning Zero Vector: Waring recursive Loop"));
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
}
