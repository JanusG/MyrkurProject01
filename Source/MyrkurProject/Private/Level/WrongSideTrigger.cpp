// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, text)
#define printf(text, fstring) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, FString::Printf(TEXT(text), fstring))

#include "Level/WrongSideTrigger.h"
#include "DrawDebugHelpers.h"
#include "Character/MyrkurProjectCharacter.h"


AWrongSideTrigger::AWrongSideTrigger()
{
	OnActorBeginOverlap.AddDynamic(this, &AWrongSideTrigger::BeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AWrongSideTrigger::EndOverlap);

	Character = NULL;
}

void AWrongSideTrigger::BeginPlay()
{
	Super::BeginPlay();
	DrawDebugBox(GetWorld(), GetActorLocation(), GetComponentsBoundingBox().GetExtent(), FColor::Red, true, -1, 0, 3);
}

void AWrongSideTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWrongSideTrigger::BeginOverlap(AActor * OverlappedActor, AActor * OtherActor)
{
	if (OtherActor && (OtherActor != this))
	{
		// Cast to character to start damage tick
		Character = Cast<AMyrkurProjectCharacter>(OtherActor);
		if (Character != nullptr)
		{
			Character->SetDamageState();
		}
	}
}

void AWrongSideTrigger::EndOverlap(AActor * OverlappedActor, AActor * OtherActor)
{
	if (OtherActor && (OtherActor != this))
	{
		// Cast to character to stop damage tick
		if (Character != NULL)
		{
			Character->SetDamageState();
			Character = NULL;
		}
	}
}