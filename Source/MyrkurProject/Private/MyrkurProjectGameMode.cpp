// Copyright Epic Games, Inc. All Rights Reserved.

// Help print debug strings
#define print(text) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Blue, text)
#define printf(text, fstring) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Blue, FString::Printf(TEXT(text), fstring))

#include "MyrkurProjectGameMode.h"
#include "MyrkurProjectGameInstance.h"
#include "MyrkurProjectHUD.h"
#include "Character/MyrkurProjectCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/Controller.h"

AMyrkurProjectGameMode::AMyrkurProjectGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// set wi

	// use our custom HUD class
	HUDClass = AMyrkurProjectHUD::StaticClass();
}

void AMyrkurProjectGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	UMyrkurProjectGameInstance* GI = Cast<UMyrkurProjectGameInstance>(GetWorld()->GetGameInstance());

	if(GI) 
	{
		FirstToWin = GI->GetFirstToWin();
	}


	PlayerCharacter = Cast<AMyrkurProjectCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	
	SetCurrentState(EGamePlayState::EPlaying);
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
		CurrentState = NewState;
		HandleNewState(NewState);
}

void AMyrkurProjectGameMode::SetStateBeginRound()
{
	SetCurrentState(EGamePlayState::ENewRound);
}

bool AMyrkurProjectGameMode::GameFinished()
{
	return bGameOver;
}

void AMyrkurProjectGameMode::AddGamePoint(bool isBlueTeam)
{
	// if the game is finished, exit function
	if(bGameOver)
		return;
	
	if(isBlueTeam){
		BlueScore++;
	}
	else
	{
		RedScore++;
	}

	numRoundsPlayed++;

	SetCurrentState(EGamePlayState::ENewRound);
}

void AMyrkurProjectGameMode::EnablePlayerInput()
{
	if(PlayerCharacter)
	{
		PlayerCharacter->EnableInput(GetWorld()->GetFirstPlayerController());
	}
}

void AMyrkurProjectGameMode::HandleNewState(EGamePlayState NewState)
{
	//Switch case to handle the state change
	switch (NewState)
	{
		
	case EGamePlayState::ENewRound:
		{
			// check if the game is finished

			// if game is finished go to game over state.
			if(BlueScore >= FirstToWin || RedScore >= FirstToWin)
			{
				print("Someone has won");
				SetCurrentState(EGamePlayState::EGameOver);
				break;
			}

			// if game is not finished start a new round
			print("Reset");
			ResetLevel();
			GetWorld()->GetAuthGameMode()->RestartPlayer(GetWorld()->GetFirstPlayerController());

			PlayerCharacter = Cast<AMyrkurProjectCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

			SetCurrentState(EGamePlayState::EPlaying);
		}
		break;
	case EGamePlayState::EPlaying:
		{
			// if either player dies, finish play
			if(PlayerCharacter)
			{
				PlayerCharacter->DisableInput(GetWorld()->GetFirstPlayerController());
			}

			FTimerHandle timeHandler;
			GetWorldTimerManager().SetTimer(timeHandler, this, &AMyrkurProjectGameMode::EnablePlayerInput, 1.5f, false);
		}
		break;
	case EGamePlayState::EGameOver:
		{
			bGameOver = true;
			// set winning/ loosing params for each player

			// if red wins, play loosing animation

			// if blue wins, play winning sequence
			PlayerCharacter->SetCanBeDamaged(false);

			// prompt player to return to menu or remach!
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
