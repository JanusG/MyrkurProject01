// Fill out your copyright notice in the Description page of Project Settings.

// Help print debug strings
#define print(text) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, text)
#define printf(text, fstring) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, FString::Printf(TEXT(text), fstring))


#include "EnemyCharacter.h"
#include "MyrkurProjectGameMode.h"
#include "MyrkurProjectProjectile.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Math/Vector.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a base mesh for the enemy
	CMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CMesh"));
	CMesh->SetupAttachment(RootComponent);
	CMesh->bCastDynamicShadow = true;
	CMesh->CastShadow = true;

	// Create a mesh component for ball that will later be connected to his hands
	BallMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BallMesh"));
	BallMesh->SetupAttachment(CMesh);
	BallMesh->SetOnlyOwnerSee(false);
	BallMesh->bCastDynamicShadow = true;
	BallMesh->CastShadow = true;
	//BallMesh->ToggleVisibility();

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FullHealth = 100.0f;
	CurrentHealth = FullHealth;
}

void AEnemyCharacter::UpdateHealth(float HealthChange)
{
	CurrentHealth += HealthChange;
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Attach to main mesh after constructor has made the bones
	BallMesh->AttachToComponent(CMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("Grip"));
}

float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UpdateHealth(-DamageAmount);

	CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, FullHealth);

	if (CurrentHealth <= 0)
	{
		// Stop Actor movements
		UCharacterMovementComponent* CharacterComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
		if (CharacterComp)
		{
			CharacterComp->StopMovementImmediately();
			CharacterComp->DisableMovement();
			CharacterComp->SetComponentTickEnabled(false);
		}

		// Play death animation
		if (DyingAnim)
		{
			// Get the animation object for the model
			UAnimInstance* AnimInstance = CMesh->GetAnimInstance();
			if (AnimInstance != nullptr)
			{
				float montageStatus = AnimInstance->Montage_Play(DyingAnim, 1.0f);
			}
		}

		FTimerHandle timeHandler;
		GetWorldTimerManager().SetTimer(timeHandler, this, &AEnemyCharacter::AddGamePoint, 2.5f, false);
	}
	else 
	{
		// If still alive, play hit animation
		if (HitgAnim)
		{
			// Get the animation object for the model
			UAnimInstance* AnimInstance = CMesh->GetAnimInstance();
			if (AnimInstance != nullptr)
			{
				float montageStatus = AnimInstance->Montage_Play(HitgAnim, 1.0f);
			}
		}
	}
	return 0.0f;
}

void AEnemyCharacter::SetCharacterRagdoll()
{
	// Set ragdoll effect
	CMesh->SetCollisionProfileName(TEXT("Ragdoll"));
	CMesh->SetSimulatePhysics(true);
}

void AEnemyCharacter::AddGamePoint()
{
	// Add gamepoint to the blue team
	AMyrkurProjectGameMode* GameMode = Cast<AMyrkurProjectGameMode>(GetWorld()->GetAuthGameMode());
	if(GameMode) 
	{
		GameMode->AddGamePoint(true);
	}
}
// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyCharacter::Reset()
{
	FVector SpawnLocation = FVector(260, -1595, 260);
	FRotator SpawnRotation = FRotator(0, 0, 0);
	CurrentHealth = 100;
	TeleportTo(SpawnLocation, SpawnRotation);

	// Re enable pawn movements
	UCharacterMovementComponent* CharacterComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (CharacterComp)
	{
		CharacterComp->SetMovementMode(EMovementMode::MOVE_Walking);
		CharacterComp->SetComponentTickEnabled(true);
	}
}

void AEnemyCharacter::PlayerLost()
{
	// Set Ragdoll 
	FTimerHandle timeHandler;
	GetWorldTimerManager().SetTimer(timeHandler, this, &AEnemyCharacter::SetCharacterRagdoll, 2.5f, false);
}

void AEnemyCharacter::InitAttack()
{
	// Play trowing animation if the animation is set
	if (TrowingAnim)
	{
		// Get the animation object for the model
		UAnimInstance* AnimInstance = CMesh->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			float montageStatus = AnimInstance->Montage_Play(TrowingAnim, 1.6f);
		}
	}

}

void AEnemyCharacter::Attack()
{
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			// calculate the distance between the two characters to make the arc of the shoot 
			// further away higher the arc

			// find player controller and get the distance
			APawn* OurPawn = UGameplayStatics::GetPlayerPawn(this, 0);
			float distance = FVector::Dist(OurPawn->GetActorLocation(), RootComponent->GetRelativeLocation());

			// set a multiplyer for the arc and the distance, first to lower the dist and then multiply to get greater variance
			float arc = (distance * 0.002f) * 1.2;

			// Set the spawn velocity
			const FRotator SpawnRotation = GetControlRotation() + FRotator(arc,0.0f, 0.0f);
			const FVector SpawnLocation = ((BallMesh != nullptr) ? BallMesh->GetComponentLocation() : GetActorLocation());

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// spawn the projectile at positon of the ball
			World->SpawnActor<AMyrkurProjectProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

			// Hide the ball until the character acquires a new one
			BallMesh->SetVisibility(false);
		}
	}
}

void AEnemyCharacter::SeteWeaponVisible()
{
	// Show curren weapon
	BallMesh->SetVisibility(true);
}

