// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraShakeBase.h"
#include "CameraShakeDamage.generated.h"

class UCameraAnim;
/**
 * 
 */
UCLASS()
class MYRKURPROJECT_API UCameraShakeDamage : public UCameraShakeBase
{
	GENERATED_BODY()
	
public:

	/** Animation for getting hit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UCameraAnim* DamageSequence;


};
