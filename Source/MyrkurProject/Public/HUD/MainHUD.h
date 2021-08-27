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

	AMyrkurProjectGameMode* GameMode;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* NumBallsLeft;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* FirstToWin;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* OutOf;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* RedScore;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* BlueScore;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UVerticalBox* VertBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Classes)
	TSubclassOf<UUserWidget> CountdownWidgetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Classes)
	TSubclassOf<UUserWidget> EndWidgetClass;

	UPROPERTY(VisibleInstanceOnly, Category = Runtime)
	URoundCountdownHUD* CountdownWidget;

	UPROPERTY(VisibleInstanceOnly, Category = Runtime)
	URoundCountdownHUD* EndWidget;

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;


};
