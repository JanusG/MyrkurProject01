// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "InteractiveObject.h"
#include "InteractGetBalls.generated.h"

class AMyrkurProjectCharacter;
/**
 * 
 */
UCLASS()
class MYRKURPROJECT_API AInteractGetBalls : public AInteractiveObject
{
	GENERATED_BODY()

public:
    /** Sets default values for this actor's properties */
    AInteractGetBalls();

    /** Called when the game starts or when spawned */
    virtual void BeginPlay() override;

    /** gives player some balls */
	void Interact();
protected:

    AMyrkurProjectCharacter* player;
private:
};
