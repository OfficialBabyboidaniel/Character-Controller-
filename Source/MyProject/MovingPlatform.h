// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GameFramework/Actor.h"
#include "MovingPlatform.generated.h"

UCLASS()
class MYPROJECT_API AMovingPlatform : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMovingPlatform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving Platform")
	TArray<AActor*> PathActors; // Array of actors representing the path

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving Platform")
	float Speed = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving Platform")
	class UBehaviorTree* BehaviorTree = nullptr; // Placeholder for Behavior Tree asset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving Platform")
	class UBlackboardComponent* BlackboardComponent = nullptr;
	
private:
	FVector StartLocation;
	FVector PathLocation;
	bool IsEmpty = true; 
};

