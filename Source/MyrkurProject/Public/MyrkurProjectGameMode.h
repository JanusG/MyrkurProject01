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
	ENewRound,
	EPlaying,
	EPaused,
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
	UFUNCTION(BlueprintPure)
	EGamePlayState GetCurrentState() const;

	/** Sets a new playing state */
	void SetCurrentState(EGamePlayState NewState);

	/** Set state to begin play */
	UFUNCTION(BlueprintCallable)
	void SetStateBeginRound();

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> CountDownWidgetClass;

	UPROPERTY()
	UUserWidget* CountDownWidget;
	
	/** 
	 * Set point to either the blu or red team
	 * @param isBlueTeam if true then blue gets the point, false the red team gets the point
	 */
	UFUNCTION(BlueprintCallable)
	void AddGamePoint(bool isBlueTeam);

protected:

	UPROPERTY(EditAnywhere, Category = Gameplay)
	int FirstToWin = 3;

	UPROPERTY(BlueprintReadOnly, Category = Gameplay)
	int numRoundsPlayed = 1;

	UPROPERTY(BlueprintReadOnly, Category = Gameplay)
	int BlueScore = 0;

	UPROPERTY(BlueprintReadOnly, Category = Gameplay)
	int RedScore = 0;

private:
	/** Keeps track of current state of game */
	EGamePlayState CurrentState;

	/** Handle setting new state of game */
	void HandleNewState(EGamePlayState NewState);
};