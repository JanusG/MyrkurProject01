// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingPlatform.generated.h"

UCLASS()
class MYRKURPROJECT_API AMovingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	/** Sets default values for this actor's properties */
	AMovingPlatform();

protected:
	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

public:	
	/** Called every frame */
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float speed;

private:

	/** Negates speed value to change direction of the Actor */
	void TurnAround();
};
