// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MainHUD.h"
#include "HUD/RoundCountdownHUD.h"
#include "Character/MyrkurProjectCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

void UMainHUD::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
    Super::NativeTick(MyGeometry, DeltaTime);

    Character = Cast<AMyrkurProjectCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

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
