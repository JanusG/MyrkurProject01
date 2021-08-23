// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyrkurProjectCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UCapsuleComponent;
class UAnimMontage;
class USoundBase;
class UCurveFloat;
class UTimelineComponent;
class ULevelSequence;
class ULevelSequencePlayer;
class AInteractiveObject;
class ALevelSequenceActor;
class AMyrkurProjectGameMode;

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

	/** Ball Mesh: Location at hands of Mesh1P */
	UPROPERTY(VisibleAnywhere, Category = Mesh)
	USkeletalMeshComponent* BallMesh;
	
	/** Signal to player if player is able to grab the ball */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	UCapsuleComponent* TriggerCapsule;

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

	/** Set damage to Actor */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void Tick(float DeltaTime) override;

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Sequence asset for player taking damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
	ULevelSequence* DamageAnimSequence;

	/** Sequence actor for player taking damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
	ALevelSequenceActor* DamageSeqActor;

	UPROPERTY()
	AActor* CatchActor;

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
	
	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* CatchBallSound;

	/** DEPRICADED
	 *  time curve for shots 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UCurveFloat* ShotCurve;

	/** Timeline for time between shots */
	UPROPERTY(EditAnywhere)
	UTimelineComponent* ShotTimeline;

	/** Base percentage for player movement .5 is walking, 1.0 is running */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float RunSpeed = 0.5;

	/** Check if red danger flash is on */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	bool isFlashingDanger;

	/** If character is in the danger zone, damage the player and play the animation on the UI */
	UFUNCTION(BlueprintPure, Category = Health)
	bool PlayDangerFlash();

	/** Bind for UI HealtBar */
	UFUNCTION(BlueprintPure, Category = Health)
	float getHealth();

	/** Get healt in text format to be shown in UI */
	UFUNCTION(BlueprintPure, Category = Health)
	FText GetHealthText();

	/** Update players health
	 * @param HealthChange Ammount og health that should change, damage or heal
	 */
	UFUNCTION(BlueprintCallable, Category = Health)
	void UpdateHealth(float HealthChange);

	/** Max ammunition that the player has */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	int MaxBallAmmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float dmgPitch = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float dmgYaw = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float dmgRoll = -0.5;

	/** Show the main player wiget */
	UFUNCTION(BlueprintCallable)
	void ShowInfoWidget();

	/** Set if Player is taking AOE damage */
	UFUNCTION()
	void SetDamageState();
	
	/** Overlap begin for trigger capsule */
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	/** Overlap end for trigger capsule */
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Reference UMG Asset in the Editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	TSubclassOf<UUserWidget> HelpWidgetClass;

	UPROPERTY(EditAnywhere)
	UUserWidget* InfoWidget;

	// Reference UMG Asset in the Editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	TSubclassOf<UUserWidget> PauseWidgetClass;

	UPROPERTY()
	UUserWidget* PauseWidget;

	UPROPERTY(EditAnywhere)
	AInteractiveObject* InteractiveObject;

	/** Give the player max ammount of balls */
	void SetBallsToMax();
protected:

	UPROPERTY()
	AMyrkurProjectGameMode* GameMode;

	/** Players full Health */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Health)
	float FullHealth;

	/** Players CUrrent Health */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Health)
	float CurrentHealth;

	/** Players Percentage Health */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Health)
	float PercentageHealth;

	/** toggle if player is on the wrong side */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Health)
	bool ShowDangerFlash;

	/** toggle if player is on the wrong side */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Health)
	bool bCanTickDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay)
	int NumberOfBallsLeft;

	/** Set if player can shoot or not */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay)
	bool bCanShoot;

	/** Set if player can catch the ball or not */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay)
	bool bCanCatch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay)
	bool bTakingDamage;

	UPROPERTY()
	FRotator PlayerInitRotation;
	
	/** Sequence player to play any levelSequence*/
	UPROPERTY()
    ULevelSequencePlayer* SequencePlayer;
	
	/** Fires a projectile. */
	void OnFire();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/** Handles sprint movements */
	void Run();
	void StopRuning();

	/** Set state of which player can shoot */
	void SetShotState();

	/** Handles event of what happenes if Action button is pressed */
	void ActionPress();

	/** Handles game pause */
	void PauseMenu();

	/** 
	* Sets the ammount of balls for the player
	* @param Ammount How many balls the player gets
	*/
	void SetBallAmmount(int Ammount);

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

	void playDamageSequence();

	void EnablePlayerInput();

	/** If the player can catch the ball add it to the hand */
	void CatchBall();
	
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
	/** Returns TriggerCapsule subobject **/
	UCapsuleComponent* GetTriggerCapsule() const { return TriggerCapsule; }

};

