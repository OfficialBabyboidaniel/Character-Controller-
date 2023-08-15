// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StateComponent.generated.h"


class APlayerCharThreeD;
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYPROJECT_API UStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UStateComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	void Enter();
	virtual void Update(float DeltaTime);
	void Exit();

	bool bShouldTick = false;

protected:
	APlayerCharThreeD* PlayerCharThreeD;

	//jump variables
	UPROPERTY(EditAnywhere, Category="Stats")
	float JumpForce = 1500.0f;
	UPROPERTY(EditAnywhere, Category="Stats")
	float GroundCheckDistance = 2.2f;

	UPROPERTY(EditAnywhere, Category="Stats")
	float SkinWidth = 2.0f;
	// use to be 1.2

	
	//vector value
	FVector Velocity = FVector::ZeroVector;
	//for update vel function
	FVector OriginalLocationBeforeUpdate;
	
	//Sweep values
	FVector Origin, Extent;
	FCollisionQueryParams Params;
	
	//camera properties
	UPROPERTY(EditAnywhere, Category="Camera Stats")
	class UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere, Category="Camera Stats")
	FVector CameraLocationRelativeToPlayer;

	//camera rotation
	UPROPERTY(VisibleAnywhere, Category="Camera Stats")
	FQuat CameraRotation;
	UPROPERTY(VisibleAnywhere, Category="Camera Stats")
	FVector EulerRotation;
	UPROPERTY(EditAnywhere, Category="Camera Stats")
	double MaxPitchRotaion = 30;
	UPROPERTY(EditAnywhere, Category="Camera Stats")
	double MinPitchRotation = -50;

	//camera input and variables
	UPROPERTY(VisibleAnywhere, Category="Camera Stats")
	FVector CameraInput = FVector::ZeroVector;
	UPROPERTY(VisibleAnywhere, Category="Camera Stats")
	double OffsetDistance;
	UPROPERTY(EditAnywhere, Category="Camera Stats")
	double MouseSensitivity = 2;
	UPROPERTY(EditAnywhere, Category="Camera Stats")
	double CameraSkinWidth = 5;

	

	void CalculatePitchInput();
	void SetInitialCameraLocation(float DeltaTime);
	void CameraCollisionCheck();
};
