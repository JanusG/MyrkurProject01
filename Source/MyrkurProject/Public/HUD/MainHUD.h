// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainHUD.generated.h"

class AMyrkurProjectCharacter;

/**
 * 
 */
UCLASS()
class MYRKURPROJECT_API UMainHUD : public UUserWidget
{
	GENERATED_BODY()

public:

protected:
	AMyrkurProjectCharacter* Character;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UProgressBar* HealthBar;
};
