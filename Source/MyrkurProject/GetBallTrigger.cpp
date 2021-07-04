// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, text)
#define printf(text, fstring) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, FString::Printf(TEXT(text), fstring))

#include "GetBallTrigger.h"
#include "DrawDebugHelpers.h"


AGetBallTrigger::AGetBallTrigger()
{
	OnActorBeginOverlap.AddDynamic(this, &AGetBallTrigger::BeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AGetBallTrigger::EndOverlap);
}

void AGetBallTrigger::BeginPlay()
{
	Super::BeginPlay();
	DrawDebugBox(GetWorld(), GetActorLocation(), GetComponentsBoundingBox().GetExtent(), FColor::Purple, true, -1, 0, 3);
}

void AGetBallTrigger::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor && (OtherActor != this))
	{
		printf("Getting balls for %s", *OtherActor->GetName());
	}
}

void AGetBallTrigger::EndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor && (OtherActor != this))
	{
		printf("Stop getting balls for %s", *OtherActor->GetName());
	}
}
