// Copyright Epic Games, Inc. All Rights Reserved.

#include "Kismet/GameplayStatics.h"
#include "MyrkurProjectGameMode.h"
#include "MyrkurProjectHUD.h"
#include "MyrkurProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

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

}

void AMyrkurProjectGameMode::HandleNewState(EGamePlayState NewState)
{
	//Switch case to handle the state change
	switch (NewState)
	{
	case EGamePlayState::EPlaying:
		{
			//
		}
		break;
	case EGamePlayState::EGameOver:
		{
			//
		}
		break;
	case EGamePlayState::EUnknown:
		{
			//
		}
		break;
	default:
		break;
	}
}
