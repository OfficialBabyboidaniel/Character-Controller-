// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class MYPROJECT_API StateMachine
{
public:
	StateMachine();
	~StateMachine();

	//protected or private?

	 TArray<class State*> States;

	//bättre namn eller ocskp missförstått ?
	void ChangeState(State* State);
	
	
};
