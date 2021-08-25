// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MainHUD.h"
#include "HUD/RoundCountdownHUD.h"
#include "Character/MyrkurProjectCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Internationalization/Text.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

void UMainHUD::NativeConstruct()
{
    Super::NativeConstruct();

    Character = Cast<AMyrkurProjectCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

    //String building
    TArray<FStringFormatArg> OutOfArgs;
    OutOfArgs.Add( FStringFormatArg(Character->GetMaxBallsAmmount()));

    // add the string to the HUD
    OutOf->SetText(FText::FromString(FString::Format( TEXT( "Out Of: {0}" ), OutOfArgs)));
}

void UMainHUD::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
    Super::NativeTick(MyGeometry, DeltaTime);


    if(Character)
    {
        // Update Healthbar percentage
        HealthBar->SetPercent(Character->getHealth());

        // Show how many balls are left 
        NumBallsLeft->SetText(Character->GetBallsLeftText());
    }
}

void UMainHUD::PlayCountdownWidget()
{
    if(CountdownWidgetClass)
    {   
        // Create widget from blueprint class
        CountdownWidget = CreateWidget<URoundCountdownHUD>(GetWorld(), CountdownWidgetClass);

        if(CountdownWidget)
        {   
            // Stores the widget inside the Vertical box
            CountdownNotification->AddChildToVerticalBox(CountdownWidget);
        }
    }
}
