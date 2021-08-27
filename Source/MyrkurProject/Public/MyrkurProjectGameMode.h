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

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> MainHUDCLass;

	/** Returns the current playing state */
	UFUNCTION(BlueprintPure)
	EGamePlayState GetCurrentState() const;

	/** Sets a new playing state */
	void SetCurrentState(EGamePlayState NewState);

	/** Set state to begin play */
	UFUNCTION(BlueprintCallable)
	void SetStateBeginRound();

	/** Returns if the game has finished or not */
	bool GameFinished();

	/** 
	 * Set point to either the blue or red team
	 * @param isBlueTeam if true then blue gets the point, false the red team gets the point
	 */
	UFUNCTION(BlueprintCallable)
	void AddGamePoint(bool isBlueTeam);

	UFUNCTION(BlueprintCallable)
	FText GetFirstToWinText();

	UFUNCTION(BlueprintCallable)
	FText GetBlueScoreText();
	
	UFUNCTION(BlueprintCallable)
	FText GetRedScoreText();

protected:

	/** pointer reference to the playerCharacter */
	UPROPERTY()
	AMyrkurProjectCharacter* PlayerCharacter;

	UPROPERTY(EditAnywhere, Category = Gameplay)
	int FirstToWin = 3;

	UPROPERTY(BlueprintReadOnly, Category = Gameplay)
	int numRoundsPlayed = 1;

	UPROPERTY(BlueprintReadOnly, Category = Gameplay)
	int BlueScore = 0;

	UPROPERTY(BlueprintReadOnly, Category = Gameplay)
	int RedScore = 0;

	UPROPERTY(BlueprintReadOnly, Category = Gameplay)
	bool bGameOver = false;

private:
	/** Keeps track of current state of game */
	EGamePlayState CurrentState;

	void EnablePlayerInput();
	
	/** Handle setting new state of game */
	void HandleNewState(EGamePlayState NewState);
};