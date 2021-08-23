// Fill out your copyright notice in the Description page of Project Settings.
// Help print debug strings
#define print(text) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, text)
#define printf(text, fstring) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, FString::Printf(TEXT(text), fstring))

#include "Enemy/ThrowNotifyState.h"
#include "Enemy/EnemyCharacter.h"
#include "Engine.h"

void UThrowNotifyState::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) 
{

}

void UThrowNotifyState::NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime) 
{

}

void UThrowNotifyState::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(MeshComp->GetOwner());
    if(Enemy != nullptr)
    {
        Enemy->SeteWeaponVisible();
    }
}