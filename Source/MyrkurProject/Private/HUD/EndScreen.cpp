// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EndScreen.h"
#include "MyrkurProjectGameMode.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UEndScreen::NativeConstruct()
{
    Super::NativeConstruct();

    AMyrkurProjectGameMode* GameMode = Cast<AMyrkurProjectGameMode>(GetWorld()->GetAuthGameMode());

    MainMenu->OnClicked.AddDynamic(this, &UEndScreen::MenuClicked);
}

void UEndScreen::MenuClicked()
{

}