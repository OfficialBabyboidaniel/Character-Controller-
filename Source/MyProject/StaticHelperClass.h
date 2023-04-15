// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class MYPROJECT_API StaticHelperClass
{
public:
	StaticHelperClass();
	~StaticHelperClass();
	static FVector DotProduct(const FVector, const FVector);
};
