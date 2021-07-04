// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "GetBallTrigger.generated.h"

class AActor;

/**
 * 
 */
UCLASS(config=Game)
class MYRKURPROJECT_API AGetBallTrigger : public ATriggerBox
{
	GENERATED_BODY()

public:
	AGetBallTrigger();

protected:
	virtual void BeginPlay();

public:

	/**
	 * Trigger Actor on enter
	* @param OverlappedActor the actor component
	* @param OtherActor incoming actor who is owerlapping
	 */
	UFUNCTION()
	void BeginOverlap(class AActor* OverlappedActor, class AActor* OtherActor);

	/** 
	* Trigger Actor on exit 
	* @param OverlappedActor the actor component
	* @param OtherActor incoming actor who is owerlapping
	*/
	UFUNCTION()
	void EndOverlap(class AActor* OverlappedActor, class AActor* OtherActor);
};
