// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	UPROPERTY(EditAnywhere, Category = "Platform")
	float Speed = 100.f; // Platform speed (units per second)

	UPROPERTY(EditAnywhere, Category = "Platform")
	FVector TargetLocation; // The target location where the platform will move

private:
	FVector GlobalStartLocation;
	FVector GlobalTargetLocation;
};
