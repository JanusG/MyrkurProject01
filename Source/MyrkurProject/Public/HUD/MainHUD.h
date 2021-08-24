// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainHUD.generated.h"

class AMyrkurProjectCharacter;
class AMyrkurProjectGameMode;

class URoundCountdownHUD;

class UProgressBar;
class UTextBlock;
class UVerticalBox;

/**
 * 
 */
UCLASS()
class MYRKURPROJECT_API UMainHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	void PlayCountdownWidget();

protected:
	AMyrkurProjectCharacter* Character;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* NumBallsLeft;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UVerticalBox* CountdownNotification;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Classes)
	TSubclassOf<UUserWidget> CountdownWidgetClass;

	UPROPERTY(VisibleInstanceOnly, Category = Runtime)
	URoundCountdownHUD* CountdownWidget;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

	
};
