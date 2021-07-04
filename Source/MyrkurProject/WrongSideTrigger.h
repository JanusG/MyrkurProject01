// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "WrongSideTrigger.generated.h"

/**
 * 
 */
UCLASS()
class MYRKURPROJECT_API AWrongSideTrigger : public ATriggerBox
{
	GENERATED_BODY()

public:
	AWrongSideTrigger();

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
