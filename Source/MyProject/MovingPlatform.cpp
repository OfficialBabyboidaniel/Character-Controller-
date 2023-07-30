#include "MovingPlatform.h"

// Sets default values
AMovingPlatform::AMovingPlatform()
{
	// Set this actor to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	GlobalStartLocation = GetActorLocation();
	GlobalTargetLocation = GetTransform().TransformPosition(TargetLocation);
}

// Called every frame
void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Ensure the platform moves only if a target location is set
	if (HasAuthority())
	{
		FVector CurrentLocation = GetActorLocation();
		FVector Direction = (GlobalTargetLocation - GlobalStartLocation).GetSafeNormal();
		float TravelDistance = (GlobalTargetLocation - GlobalStartLocation).Size();
		float TravelDelta = Speed * DeltaTime;

		if ((CurrentLocation - GlobalStartLocation).Size() >= TravelDistance)
		{
			// Swap start and target locations to move the platform back and forth
			FVector Temp = GlobalStartLocation;
			GlobalStartLocation = GlobalTargetLocation;
			GlobalTargetLocation = Temp;
		}

		SetActorLocation(CurrentLocation + Direction * TravelDelta);
	}
}
