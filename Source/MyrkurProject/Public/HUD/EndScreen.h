// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndScreen.generated.h"

class UTextBlock;
class UButton;

/**
 * 
 */
UCLASS()
class MYRKURPROJECT_API UEndScreen : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Winner;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* MainMenu;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* Restart;

	virtual void NativeConstruct() override;

	void MenuClicked();
};