// Fill out your copyright notice in the Description page of Project Settings.


#include "State.h"
#include "Statemachine.h"

State::State(StateMachine& InStateMachineRef) : StateMachineRef(InStateMachineRef)
{
	StateMachineRef.States.Add(this);
}

State::~State()
{
}

