// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractGetBalls.h"

AInteractGetBalls::AInteractGetBalls()
{
	// get self referece for dynamic linkage
	AInteractGetBalls* self = &(*this);
	self->Interact();
}

void AInteractGetBalls::BeginPlay()
{
	Super::BeginPlay();
}

void AInteractGetBalls::Interact()
{

}