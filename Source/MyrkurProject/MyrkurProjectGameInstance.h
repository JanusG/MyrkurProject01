// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyrkurProjectGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MYRKURPROJECT_API UMyrkurProjectGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	private:
		
		int FirstToWin = 3;
	public:

		UFUNCTION(BlueprintCallable)
		int GetFirstToWin();

		UFUNCTION(BlueprintCallable)
		void SetFirstToWin(int games);

};
