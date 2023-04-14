// Fill out your copyright notice in the Description page of Project Settings.

#include "StaticHelperClass.h"

StaticHelperClass::StaticHelperClass()
{
}

StaticHelperClass::~StaticHelperClass()
{
}

FVector StaticHelperClass::DotProduct(FVector Velocity, FVector Normal)
{
    if (FVector::DotProduct(Velocity, Normal) > 0)
    {
        return FVector::ZeroVector;
    }
    else
    {
        FVector Projection = FVector::DotProduct(Velocity, Normal) * Normal;
        return -Projection;
    }
}
