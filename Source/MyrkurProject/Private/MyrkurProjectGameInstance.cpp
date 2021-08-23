// Fill out your copyright notice in the Description page of Project Settings.


#include "MyrkurProjectGameInstance.h"

int UMyrkurProjectGameInstance::GetFirstToWin()
{
    return FirstToWin;
}

void UMyrkurProjectGameInstance::SetFirstToWin(int games)
{
    FirstToWin = games;
}