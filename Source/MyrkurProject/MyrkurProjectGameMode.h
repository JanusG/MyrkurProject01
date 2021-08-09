// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyrkurProjectGameMode.generated.h"

class AMyrkurProjectCharacter;

//Enum for current state of the game
UENUM()
enum class EGamePlayState
{
	EPlaying,
	EGameOver,
	EUnknown
};

UCLASS(minimalapi)
class AMyrkurProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyrkurProjectGameMode();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	/** pointer reference to the playerCharacter */
	AMyrkurProjectCharacter* PlayerCharacter;

	/** Returns the current playing state */
	UFUNCTION(BlueprintPure, Category = "Health")
	EGamePlayState GetCurrentState() const;

	/** Sets a new playing state */
	void SetCurrentState(EGamePlayState NewState);

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> HUDWidgetClass;

	UPROPERTY(EditAnywhere)
	class UUserWidget* MainMenuWidget;

private:
	/** Keeps track of current state of game */
	EGamePlayState CurrentState;

	/** Handle setting new state of game */
	void HandleNewState(EGamePlayState NewState);
};



