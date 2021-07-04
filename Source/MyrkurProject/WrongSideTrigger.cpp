// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, text)
#define printf(text, fstring) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, FString::Printf(TEXT(text), fstring))

#include "WrongSideTrigger.h"


AWrongSideTrigger::AWrongSideTrigger()
{
	OnActorBeginOverlap.AddDynamic(this, &AWrongSideTrigger::BeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AWrongSideTrigger::EndOverlap);
}

void AWrongSideTrigger::BeginPlay()
{
	Super::BeginPlay();
	//DrawDebugBox(GetWorld(), GetActorLocation(), GetComponentsBoundingBox().GetExtent(), FColor::Purple, true, -1, 0, 3);
}

void AWrongSideTrigger::BeginOverlap(AActor * OverlappedActor, AActor * OtherActor)
{
	if (OtherActor && (OtherActor != this))
	{
		
	}
}

void AWrongSideTrigger::EndOverlap(AActor * OverlappedActor, AActor * OtherActor)
{
	if (OtherActor && (OtherActor != this))
	{
		
	}
}