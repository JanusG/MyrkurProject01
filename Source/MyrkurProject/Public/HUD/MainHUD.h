// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainHUD.generated.h"

/**
 * 
 */
UCLASS()
class MYRKURPROJECT_API UMainHUD : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UProgressBar* HealthBar;
};
