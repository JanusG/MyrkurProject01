// Copyright Epic Games, Inc. All Rights Reserved.

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
