// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "MyrkurProjectCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config=Game)
class AMyrkurProjectCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FP_Gun;

	UPROPERTY(VisibleAnywhere, Category = Mesh)
	USkeletalMeshComponent* BallMesh;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* FP_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

public:
	AMyrkurProjectCharacter();

protected:
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;
public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AMyrkurProjectProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** Base percentage for player movement .5 is walking, 1.0 is running */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float RunSpeed = 0.5;

	/** If character is in the danger zone, damage the player and play the animation on the UI */
	UFUNCTION(BlueprintPure, Category = Health)
	bool PlayDangerFlash();

	/** Bind for UI HealtBar */
	UFUNCTION(BlueprintPure, Category = Health)
	float getHealth();

	/** Get healt in text format to be shown in UI */
	UFUNCTION(BlueprintPure, Category = Health)
	FText GetHealthText();

	/** Update players health */
	UFUNCTION(BlueprintCallable, Category = Health)
	void UpdateHealth(float HealthChange);

	UFUNCTION()
	void SetDamageState();

	void ReceivePointDamage(float Damage, const class UDamageType* DamageType, FVector HitLocation, FVector HitNormal, class UPrimitiveComponent* HitComponent, FName BoneName, FVector ShotFromDirection, class AController* InstigatedBy, AActor* DamageCauser, const FHitResult& HitInfo);

	UFUNCTION(BlueprintCallable, Category = Health)
	void DamageTimer();

protected:

	/** Players full Health */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Health)
		float FullHealth;

	/** Players CUrrent Health */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Health)
		float CurrentHealth;

	/** Players Percentage Health */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Health)
		float PercentageHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay)
		int NumberOfBallsLeft;

	/** toggle if player is on the wrong side */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Health)
		bool ShowDangerFlash;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool bCanShoot;
	
	/** Fires a projectile. */
	void OnFire();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/** Handles sprint movements */
	void Run();
	void StopRuning();

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns  BallMesh subobject */
	USkeletalMeshComponent* GetBallMesh() const { return BallMesh; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
};

