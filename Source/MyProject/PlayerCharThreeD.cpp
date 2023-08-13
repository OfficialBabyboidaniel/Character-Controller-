// Fill out your copyright notice in the Description page of Project Settings.
#include "PlayerCharThreeD.h"
#include "StateMachineComponent.h"
#include "Components/InputComponent.h"


// Sets default values
APlayerCharThreeD::APlayerCharThreeD()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	StateMachineComponent = CreateDefaultSubobject<UStateMachineComponent>(TEXT("StateMachine"));
}

// Called when the game starts or when spawned
void APlayerCharThreeD::BeginPlay()
{
	Super::BeginPlay();

	if (StateMachineComponent)
	{
		if (StateMachineComponent->States.Num() > 0)
		{
			StateMachineComponent->ChangeState(StateMachineComponent->States[0]);
			UE_LOG(LogTemp, Warning, TEXT("StateMachine changed state correctly"));
		}
		//states null, we try adding airstate and see what happens
		UE_LOG(LogTemp, Warning, TEXT("StateMachine created correctly"));
	}
}

// Called every frame
/*void APlayerCharThreeD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}*/

// Called to bind functionality to input
void APlayerCharThreeD::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// bind inputs
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharThreeD::JumpInput);
	PlayerInputComponent->BindAxis("X", this, &APlayerCharThreeD::XInput);
	PlayerInputComponent->BindAxis("Y", this, &APlayerCharThreeD::YInput);
	PlayerInputComponent->BindAxis("LookRight", this, &APlayerCharThreeD::LookRight);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharThreeD::LookUp);
}

void APlayerCharThreeD::XInput(float AxisValue)
{
	CurrentInput = FVector(AxisValue, CurrentInput.Y, CurrentInput.X);
}

void APlayerCharThreeD::YInput(float AxisValue)
{
	CurrentInput = FVector(CurrentInput.X, AxisValue, CurrentInput.Z);
}

void APlayerCharThreeD::LookRight(float AxisValue)
{
	YawAxisValue = AxisValue;
}

void APlayerCharThreeD::LookUp(float AxisValue)
{
	PitchAxisValue = AxisValue;
}

void APlayerCharThreeD::JumpInput()
{
	GetActorBounds(true, Origin, Extent);
	Params.AddIgnoredActor(this);
	FHitResult Hit;
	FVector TraceEnd = Origin + FVector::DownVector * (GroundCheckDistance + SkinWidth);
	bool bHit = GetWorld()->SweepSingleByChannel(Hit, Origin, TraceEnd, FQuat::Identity, ECC_Pawn,
	                                             FCollisionShape::MakeCapsule(Extent), Params);
	if (bHit)
	{
		JumpMovement = FVector(0, 0, JumpForce);
	}
	UE_LOG(LogTemp, Warning, TEXT("Jumpiong"));
}

//get functions
FVector APlayerCharThreeD::GetCurrentInput() const
{
	return CurrentInput;
}

FVector APlayerCharThreeD::GetJumpInput() const
{
	return JumpMovement;
}

float APlayerCharThreeD::GetPitchAxisValue() const
{
	return PitchAxisValue;
}

float APlayerCharThreeD::GetYawAxisValue() const
{
	return YawAxisValue;
}

//set functions
void APlayerCharThreeD::SetCurrentInput(const FVector NewValue)
{
	CurrentInput = NewValue;
}

void APlayerCharThreeD::SetJumpInput(const FVector NewValue)
{
	JumpMovement = NewValue;
}
