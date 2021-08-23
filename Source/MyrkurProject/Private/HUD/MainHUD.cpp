// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MainHUD.h"
#include "Character/MyrkurProjectCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ProgressBar.h"


void UMainHUD::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
    Super::NativeTick(MyGeometry, DeltaTime);

    Character = Cast<AMyrkurProjectCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

    if(Character)
    {
        HealthBar->SetPercent(Character->getHealth());
    }

}
