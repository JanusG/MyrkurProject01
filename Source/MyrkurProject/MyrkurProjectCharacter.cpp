// Copyright Epic Games, Inc. All Rights Reserved.

// Help print debug strings
#define print(text) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, text)
#define printf(text, fstring) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, FString::Printf(TEXT(text), fstring))

#include "MyrkurProjectCharacter.h"
#include "MyrkurProjectProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/TimelineComponent.h"
#include "Blueprint/UserWidget.h"
#include "DrawDebugHelpers.h"
#include "InteractiveObject.h"
#include "math.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AMyrkurProjectCharacter

AMyrkurProjectCharacter::AMyrkurProjectCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	FP_Gun->SetupAttachment(Mesh1P);
	FP_Gun->SetRelativeScale3D(FVector(-0.75f, -0.75f, -0.75f));
	FP_Gun->SetRelativeLocation(FVector(100.0f, 0.0f, 100.0f));
	FP_Gun->SetupAttachment(RootComponent);

	// Create a mesh component for ball
	BallMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BallMesh"));
	BallMesh->SetOnlyOwnerSee(true);
	BallMesh->bCastDynamicShadow = false;
	BallMesh->CastShadow = false;
	//Ball->SetupAttachment(Mesh1P);
	BallMesh->SetupAttachment(Mesh1P, TEXT("LeftGrip"));

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 40.0f, 5.0f);

	bCanShoot = true;

	MaxBallAmmount = 5;
	NumberOfBallsLeft = MaxBallAmmount;

	FullHealth = 100.0f;
	CurrentHealth = FullHealth;
	PercentageHealth = 1;

	ShowDangerFlash = false;

	InteractiveObject = NULL;

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.
}

void AMyrkurProjectCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Initialize time curve for shots
	if (ShotCurve)
	{
		FOnTimelineFloat TimelineCallback;
		FOnTimelineEventStatic TimelineFinishedCallback;

		TimelineCallback.BindUFunction(this, FName("OnFire"));
		//TimelineFinishedCallback.BindUFunction(this, FName("SetShotState"));

		ShotTimeline = NewObject<UTimelineComponent>(this, FName("ShotTimer"));
		ShotTimeline->AddInterpFloat(ShotCurve, TimelineCallback);
		//ShotTimeline->SetTimelineFinishedFunc(TimelineFinishedCallback);
		ShotTimeline->RegisterComponent();
	}


	// Show UI components
	if (HelpWidgetClass) 
	{
		// Initialize the info UI and set parameters 
		InfoWidget = CreateWidget<UUserWidget>(GetWorld(), HelpWidgetClass);

		if (InfoWidget)
		{
			InfoWidget->AddToViewport();
			// Hide Info text that should not be shown
			InfoWidget->GetWidgetFromName("textInteract")->SetVisibility(ESlateVisibility::Hidden);
			InfoWidget->GetWidgetFromName("TextDeath")->SetVisibility(ESlateVisibility::Hidden);
			InfoWidget->GetWidgetFromName("BlurDeath")->SetVisibility(ESlateVisibility::Hidden);
			InfoWidget->GetWidgetFromName("Danger")->SetVisibility(ESlateVisibility::Hidden);

			// Hidden while the function isn't ready so not to confuse the player
			// InfoWidget->GetWidgetFromName("HealthBar")->SetVisibility(ESlateVisibility::Hidden);

		}
	}
	
	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("LeftGrip"));
}

float AMyrkurProjectCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UpdateHealth(-DamageAmount);
	return 0.0f;
}

void AMyrkurProjectCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ShotTimeline != NULL)
	{
		ShotTimeline->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, NULL);
	}

	// LineTrace to search for interactable components in the scene
	FHitResult Hit;
	FVector Start = FirstPersonCameraComponent->GetComponentLocation();

	FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();
	FVector End = ((ForwardVector * 200.f) + Start);
	FCollisionQueryParams CollisionParams;

	// Check if interactable component is in front of player, and if so toggle info message.
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionParams))
	{
		if (Hit.bBlockingHit)
		{
			if (Hit.GetActor()->GetClass()->IsChildOf(AInteractiveObject::StaticClass()))
			{
				InfoWidget->GetWidgetFromName("textInteract")->SetVisibility(ESlateVisibility::Visible);
				InteractiveObject = Cast<AInteractiveObject>(Hit.GetActor());
			}
		}
	}
	else
	{
		InfoWidget->GetWidgetFromName("textInteract")->SetVisibility(ESlateVisibility::Hidden);
		InteractiveObject = NULL;
	}

	// if character can be damaged by AOE the set the damage and show danger screen
	if (bCanTickDamage)
	{
		UpdateHealth(-0.25f);
	}

}

//////////////////////////////////////////////////////////////////////////
// Input

void AMyrkurProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMyrkurProjectCharacter::OnFire);

	// Bind action event
	PlayerInputComponent->BindAction("Action", IE_Pressed, this, &AMyrkurProjectCharacter::ActionPress);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AMyrkurProjectCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyrkurProjectCharacter::MoveRight);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AMyrkurProjectCharacter::Run);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &AMyrkurProjectCharacter::StopRuning);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMyrkurProjectCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMyrkurProjectCharacter::LookUpAtRate);
}

void AMyrkurProjectCharacter::OnFire()
{
	// if there are any balls left, try and fire a projectile
	if (ProjectileClass != nullptr && bCanShoot)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			const FRotator SpawnRotation = GetControlRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// spawn the projectile at the muzzle
			World->SpawnActor<AMyrkurProjectProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}

		// try and play the sound if specified
		if (FireSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}

		// try and play a firing animation if specified
		if (FireAnimation != nullptr)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
			if (AnimInstance != nullptr)
			{
				AnimInstance->Montage_Play(FireAnimation, 0.3f);
			}
		}

		// Start timer for shots
		if (ShotTimeline != NULL)
		{
			ShotTimeline->PlayFromStart();
		}

		// set number of balls, and if zero set character to not to be able to shoot
		NumberOfBallsLeft--;

		if (NumberOfBallsLeft <= 0)
		{
			bCanShoot = false;
			BallMesh->SetVisibility(false);
		}
	}
}

void AMyrkurProjectCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value * RunSpeed);
	}
}

void AMyrkurProjectCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value * RunSpeed);
	}
}

void AMyrkurProjectCharacter::Run()
{	
	// Set running speed
	RunSpeed = 1.0;
}


void AMyrkurProjectCharacter::StopRuning()
{
	// Set running speed
	RunSpeed = 0.5;
}

void AMyrkurProjectCharacter::SetShotState()
{
	if (NumberOfBallsLeft > 0)
	{
		bCanShoot = true;
		BallMesh->SetVisibility(true);
	}
}

void AMyrkurProjectCharacter::ActionPress()
{

	if (InteractiveObject != NULL)
	{
		InteractiveObject->Interact();
	}
}

void AMyrkurProjectCharacter::SetBallsToMax()
{
		SetBallAmmount(MaxBallAmmount);
}

void AMyrkurProjectCharacter::SetBallAmmount(int Ammount)
{
	NumberOfBallsLeft = Ammount;
	SetShotState();
}

void AMyrkurProjectCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMyrkurProjectCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AMyrkurProjectCharacter::PlayDangerFlash()
{
	// if already flashing then return true and reset check
	if(isFlashingDanger)
	{
		isFlashingDanger = false;
		return true;
	}
	return false;
}

float AMyrkurProjectCharacter::getHealth()
{
	return PercentageHealth;
}

FText AMyrkurProjectCharacter::GetHealthText()
{
	// Round to nearest zero
	int32 HP = FMath::RoundHalfFromZero(PercentageHealth* 100);
	FString HPS = FString::FromInt(HP);
	FString HealthHUD = HPS + FString(TEXT("%"));
	FText HPText = FText::FromString(HealthHUD);

	return HPText;
}

void AMyrkurProjectCharacter::UpdateHealth(float HealthChange)
{
	CurrentHealth += HealthChange;
	CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, FullHealth);
	PercentageHealth = CurrentHealth / FullHealth;

	// Pause the game if you died and show sime kind of death screen
	if (CurrentHealth <= 0)
	{
		APlayerController* PController = Cast<APlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));

		if (PController != nullptr)
		{
			PController->SetPause(true);
		}

		// Show info about your death
		InfoWidget->GetWidgetFromName("TextDeath")->SetVisibility(ESlateVisibility::Visible);
		InfoWidget->GetWidgetFromName("BlurDeath")->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMyrkurProjectCharacter::SetDamageState()
{
	// toggle damage tick
	bCanTickDamage = !bCanTickDamage;

	// toggle info about danger if in the danger zone
	if (bCanTickDamage) 
	{
		InfoWidget->GetWidgetFromName("Danger")->SetVisibility(ESlateVisibility::Visible);
		return;
	}
	InfoWidget->GetWidgetFromName("Danger")->SetVisibility(ESlateVisibility::Hidden);
}

