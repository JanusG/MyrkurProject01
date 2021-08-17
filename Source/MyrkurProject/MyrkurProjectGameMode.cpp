// Copyright Epic Games, Inc. All Rights Reserved.

// Help print debug strings
#define print(text) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Blue, text)
#define printf(text, fstring) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Blue, FString::Printf(TEXT(text), fstring))

#include "MyrkurProjectGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "MyrkurProjectHUD.h"
#include "MyrkurProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/Controller.h"

AMyrkurProjectGameMode::AMyrkurProjectGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AMyrkurProjectHUD::StaticClass();

	
}

void AMyrkurProjectGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	print("New Game");

	SetCurrentState(EGamePlayState::EPlaying);

	PlayerCharacter = Cast<AMyrkurProjectCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
}

void AMyrkurProjectGameMode::Tick(float DeltaTime)
{

}

EGamePlayState AMyrkurProjectGameMode::GetCurrentState() const
{
	return CurrentState;
}

void AMyrkurProjectGameMode::SetCurrentState(EGamePlayState NewState)
{
	HandleNewState(NewState);
}

void AMyrkurProjectGameMode::SetStateBeginRound()
{
	SetCurrentState(EGamePlayState::ENewRound);
}

void AMyrkurProjectGameMode::AddGamePoint(bool isBlueTeam)
{
	if(isBlueTeam){
		BlueScore++;
	}
	else
	{
		RedScore++;
	}

	SetCurrentState(EGamePlayState::ENewRound);
}

void AMyrkurProjectGameMode::HandleNewState(EGamePlayState NewState)
{
	//Switch case to handle the state change
	switch (NewState)
	{
		
	case EGamePlayState::ENewRound:
		{
			print("New Round");
			// check if the game is finished

			// if game is finished go to game over state.
			if(BlueScore >= FirstToWin || RedScore >= FirstToWin)
			{
				SetCurrentState(EGamePlayState::EGameOver);
			}

			// if game is not finished start a new round
			ResetLevel();
			GetWorld()->GetAuthGameMode()->RestartPlayer(GetWorld()->GetFirstPlayerController());
			//UGameplayStatics::SetGamePaused(this, true);
		}
		break;
	case EGamePlayState::EPlaying:
		{
			// if either player dies, finish play
		}
		break;
	case EGamePlayState::EGameOver:
		{
			// set winning/ loosing params for each player
		}
		break;
	case EGamePlayState::EUnknown:
		{
			// Not Used
		}
		break;
	default:
		break;
	}
}
