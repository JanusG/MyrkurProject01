// Copyright Epic Games, Inc. All Rights Reserved.

// Help print debug strings
#define print(text) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Orange, text)
#define printf(text, fstring) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Orange, FString::Printf(TEXT(text), fstring))

#include "MyrkurProjectCharacter.h"
#include "MyrkurProjectProjectile.h"
#include "MyrkurProjectGameMode.h"
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
#include "LevelSequencePlayer.h"
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

	// Set up for grab distance trigger
	TriggerCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("TriggerSphere"));
	TriggerCapsule->InitCapsuleSize(1.0f, 1.0f);
	TriggerCapsule->BodyInstance.SetCollisionProfileName("Trigger");
	TriggerCapsule->OnComponentBeginOverlap.AddDynamic(this, &AMyrkurProjectCharacter::OnOverlapBegin);
	TriggerCapsule->OnComponentEndOverlap.AddDynamic(this, &AMyrkurProjectCharacter::OnOverlapEnd);
	TriggerCapsule->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));
	
	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 40.0f, 5.0f);

	bCanShoot = true;
	bCanCatch = false;

	MaxBallAmmount = 3;
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
	
	// init GameMode variable here for future references
	GameMode = Cast<AMyrkurProjectGameMode>(GetWorld()->GetAuthGameMode());

	// Show main HUD
	ShowInfoWidget();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("LeftGrip"));
}

float AMyrkurProjectCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UpdateHealth(-DamageAmount);

	// play animation
	playDamageSequence();
	
	// Set so the player cant catch the ball after he gets hit
	bCanCatch = false;

	return 1.0f;
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

	if(InfoWidget) 
	{
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
	}
	

	// if character can be damaged by AOE the set the damage and show danger screen
	if (bCanTickDamage)
	{
		UpdateHealth(-0.25f);
	}

	// if the player has gotten shot then play short animation
	if(bTakingDamage)
	{	
		// rotator for the animation, values are finetuned in blueprint
		FRotator NewRotation = FRotator(dmgPitch, dmgYaw, dmgRoll);
		FQuat QuatRotation = FQuat(NewRotation);

		AddActorLocalRotation(QuatRotation, false, 0, ETeleportType::None);
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

	// Bind pause event
	PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &AMyrkurProjectCharacter::PauseMenu);

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
	print("Pressing interact");
	if (InteractiveObject != NULL)
	{	
		print("Getting balls");
		InteractiveObject->Interact();
	}

	if(bCanCatch)
	{
		print("catching ball");
		CatchBall();
	}
}

void AMyrkurProjectCharacter::PauseMenu()
{
	
	APlayerController* PController = Cast<APlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	if (PController != nullptr)
	{
		PController->SetPause(true);
		PController->bShowMouseCursor = true;
		PController->bEnableMouseOverEvents = true;
		PController->bEnableClickEvents = true;
	}
	if (PauseWidgetClass) 
	{
		// Initialize pause widget
		PauseWidget = CreateWidget<UUserWidget>(GetWorld(), PauseWidgetClass);
		if (PauseWidget)	
		{
			PauseWidget->AddToViewport();
		}
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

	// Pause the game if you died and show death animation
	if (CurrentHealth <= 0)
	{
		APlayerController* PController = Cast<APlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));

		if (PController != nullptr)
		{
			//PController->SetPause(true);
		}

		if(GameMode) 
		{
			GameMode->AddGamePoint(false);
		}

		// Show info about your death
		// InfoWidget->GetWidgetFromName("TextDeath")->SetVisibility(ESlateVisibility::Visible);
		// InfoWidget->GetWidgetFromName("BlurDeath")->SetVisibility(ESlateVisibility::Visible);
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

void AMyrkurProjectCharacter::ShowInfoWidget()
{
	
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
}

void AMyrkurProjectCharacter::playDamageSequence()
{
	//disable controls while animation is being played
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	DisableInput(PlayerController);
	
	bTakingDamage = true;
	PlayerInitRotation = GetActorRotation();

	FTimerHandle timeHandler;
	GetWorldTimerManager().SetTimer(timeHandler, this, &AMyrkurProjectCharacter::EnablePlayerInput, 0.25f, false);
}

void AMyrkurProjectCharacter::EnablePlayerInput()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	EnableInput(PlayerController);

	//Reset rotation to the same as it was before hit
	SetActorRotation(PlayerInitRotation);
	bTakingDamage = false;
}


void AMyrkurProjectCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//Run if player has no ball in hand then the player has a chance of catching
	print("isOverlapping");
	if(NumberOfBallsLeft <= 0)
	{
		//TODO:: get forward wector of both items and compare if they are facing each other with dot product
		if (OtherActor && (OtherActor != this) && OtherComp) 
		{
			FVector BallVector = OtherActor->GetActorForwardVector();
			FVector PlayerVector = this->GetActorForwardVector();

			float Dot = FVector::DotProduct(BallVector, PlayerVector);
			
			//if the player is looking at the ball then he can catch it
			if(Dot < -0.980f)
			{
				//Play sound to notify the player that he can catch the ball
				if (CatchBallSound != nullptr)
				{
					UGameplayStatics::PlaySoundAtLocation(this, CatchBallSound, GetActorLocation());
				}
				//TODO:: if input is recieved before the ball hits or leaves hitbox, Delete the incoming ball and add it to the player.
				CatchActor = OtherActor;
				bCanCatch = true;
			}
		}
	}
}

void AMyrkurProjectCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(bCanCatch)
	{
		OtherActor = nullptr;
		bCanCatch = false;
	}
}

void AMyrkurProjectCharacter::CatchBall() {

	//if input is recieved before the ball hits or leaves hitbox, Delete the incoming ball and add it to the player.
	if(bCanCatch && CatchActor != nullptr)
	{
		FVector BallVector = CatchActor->GetActorForwardVector();
		FVector PlayerVector = this->GetActorForwardVector();
		
		float Dot = FVector::DotProduct(BallVector, PlayerVector);
		// Check if the ball is behind the player befor catching		
		if(Dot < -0.600f)
		{
			SetBallAmmount(1);
			CatchActor->Destroy();
			bCanCatch = false;
		}
		
	}
}

	
