// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyrkurProjectHUD.generated.h"

UCLASS()
class AMyrkurProjectHUD : public AHUD
{
	GENERATED_BODY()

public:
	AMyrkurProjectHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

