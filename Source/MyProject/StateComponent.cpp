// Fill out your copyright notice in the Description page of Project Settings.


#include "StateComponent.h"

#include "PlayerCharThreeD.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UStateComponent::UStateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UStateComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Cast<APlayerCharThreeD>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		PlayerCharThreeD = Cast<APlayerCharThreeD>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	}

	Camera = PlayerCharThreeD->FindComponentByClass<UCameraComponent>();
	if (Camera)
	{
		CameraLocationRelativeToPlayer = Camera->GetComponentLocation();
		// get distance between player and camera
		OffsetDistance = FVector::Distance(PlayerCharThreeD->GetActorLocation(), CameraLocationRelativeToPlayer);
	}
	
	Params.AddIgnoredActor(PlayerCharThreeD);
}


// Called every frame
void UStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
	CameraInput.Z += PlayerCharThreeD->GetYawAxisValue() * MouseSensitivity;
	CameraInput.Y += PlayerCharThreeD->GetPitchAxisValue() * MouseSensitivity;
	CalculatePitchInput();
	CameraRotation = FQuat::MakeFromEuler(CameraInput);
	if (Camera)
	{
		SetInitialCameraLocation(DeltaTime);
		CameraCollisionCheck();
	}
	
}

void UStateComponent::Enter()
{
	bShouldTick = true;
}

void UStateComponent::Update(float DeltaTime)
{
	
}

void UStateComponent::Exit()
{
	bShouldTick = false;
}

//Camera
void UStateComponent::CalculatePitchInput()
{
	CameraRotation = FQuat::MakeFromEuler(CameraInput);
	EulerRotation = CameraRotation.Euler();
	//clamp pitch value if reached max/min rotaion
	if (EulerRotation.Y > MaxPitchRotaion || EulerRotation.Y < MinPitchRotation)
	{
		EulerRotation.Y = FMath::Clamp(EulerRotation.Y, MinPitchRotation, MaxPitchRotaion);
		//remove value so it does not add to the input forever, prevents a 180* spin 
		CameraInput.Y -= PlayerCharThreeD->GetPitchAxisValue() * MouseSensitivity;
		CameraRotation.Y = EulerRotation.Y;
	}
}

void UStateComponent::SetInitialCameraLocation(float DeltaTime)
{
	CameraRotation *= DeltaTime;
	Camera->SetRelativeRotation(CameraRotation);
	FVector OffsetDirection = -Camera->GetRelativeRotation().Quaternion().GetForwardVector();
	OffsetDirection *= OffsetDistance;
	Camera->SetRelativeLocation(OffsetDirection);
}

void UStateComponent::CameraCollisionCheck()
{
	FHitResult CameraHit;
	PlayerCharThreeD->GetActorBounds(true, Origin, Extent);
	FVector TraceStart = Origin;
	FVector TraceEnd = Camera->GetComponentLocation();
	bool CameraSweep = GetWorld()->SweepSingleByChannel(CameraHit, TraceStart, TraceEnd, FQuat::Identity, ECC_Pawn,
														FCollisionShape::MakeSphere(CameraSkinWidth * 2), Params);
	if (CameraSweep)
	{
		FVector NewOffsetDirection = -Camera->GetRelativeRotation().Quaternion().
											  GetForwardVector();
		Camera->SetRelativeLocation(
			NewOffsetDirection *= (CameraHit.Distance - CameraSkinWidth));
	}
}
