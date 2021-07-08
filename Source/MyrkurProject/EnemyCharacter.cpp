// Fill out your copyright notice in the Description page of Project Settings.

// Help print debug strings
#define print(text) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, text)
#define printf(text, fstring) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, FString::Printf(TEXT(text), fstring))


#include "EnemyCharacter.h"
#include "Components/CapsuleComponent.h"
#include "MyrkurProjectProjectile.h"
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
}

void AEnemyCharacter::ReceivePointDamage(float Damage, const UDamageType* DamageType, FVector HitLocation, FVector HitNormal, UPrimitiveComponent* HitComponent, FName BoneName, FVector ShotFromDirection, AController* InstigatedBy, AActor* DamageCauser, const FHitResult& HitInfo)
{
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Attach to main mesh after constructor has made the bones
	BallMesh->AttachToComponent(CMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("Grip"));
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

			// TODO :
			// find player controller and get the distance
			APawn* OurPawn = UGameplayStatics::GetPlayerPawn(this, 0);
			float distance = GetDistanceTo(OurPawn);
			float arc = distance * 0.2f;
			FString TheFloatStr = FString::SanitizeFloat(distance);
			printf("%d", *TheFloatStr);

			const FRotator SpawnRotation = GetControlRotation() + FRotator(15,0.0f, 0.0f);
			const FVector SpawnLocation = ((BallMesh != nullptr) ? BallMesh->GetComponentLocation() : GetActorLocation());

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// spawn the projectile at positon of the ball
			World->SpawnActor<AMyrkurProjectProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}

		// Play trowing animation if the animation is set
		if (TrowingAnim)
		{
			PlayAnimMontage(TrowingAnim, 5.0f, NAME_None);
		}
	}
}

