// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class MYPROJECT_API State
{
public:
	State(class StateMachine& StateMachineRef);
	~State();


	void Enter();
	void Update();
	void Exit();

protected:
	 class StateMachine& StateMachineRef;
};
