// Fill out your copyright notice in the Description page of Project Settings.

#include "StaticHelperClass.h"

FVector StaticHelperClass::DotProduct(FVector Velocity, FVector Normal)
{
    float DotProduct = FVector::DotProduct(Velocity, Normal);
	if (DotProduct > 0) DotProduct = 0;
	return -(DotProduct * Normal);
}
