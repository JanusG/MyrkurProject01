// Fill out your copyright notice in the Description page of Project Settings.

// Help print debug strings
#define print(text) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, text)
#define printf(text, fstring) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, FString::Printf(TEXT(text), fstring))


#include "InteractiveObject.h"
AInteractiveObject::AInteractiveObject()
{
}

void AInteractiveObject::BeginPlay()
{
    Super::BeginPlay();
}

void AInteractiveObject::Interact()
{
}
