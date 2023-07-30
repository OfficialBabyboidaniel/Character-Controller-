#include "MovingPlatform.h"

#include "BehaviorTree/BehaviorTree.h"


AMovingPlatform::AMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();

	if(PathActors.Num() > 0 && BehaviorTree != nullptr && BlackboardComponent != nullptr )
	{
		IsEmpty = false;
	}
	
}

void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!IsEmpty)
	{
		//BlackboardComponent->SetValueAsVector("PathLocation", PathActors[0]->GetActorLocation());
	}
}
