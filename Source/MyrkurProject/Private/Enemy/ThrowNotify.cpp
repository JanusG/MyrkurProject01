// Fill out your copyright notice in the Description page of Project Settings.

// Help print debug strings
#define print(text) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Orange, text)
#define printf(text, fstring) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Orange, FString::Printf(TEXT(text), fstring))

#include "Enemy/ThrowNotify.h"
#include "Enemy/EnemyCharacter.h"
#include "Engine.h"

void UThrowNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(MeshComp->GetOwner());
    if(Enemy != nullptr)
    {
        // Attack player at that point of the animation
        Enemy->Attack();
    }
}