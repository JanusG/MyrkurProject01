// Copyright Epic Games, Inc. All Rights Reserved.

// Help print debug strings
#define print(text) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, text)
#define printf(text, fstring) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, FString::Printf(TEXT(text), fstring))

#include "MyrkurProjectProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MyrkurProjectCharacter.h"

AMyrkurProjectProjectile::AMyrkurProjectProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(24.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AMyrkurProjectProjectile::OnHit);		// set up a notification for when this component hits something blocking


	// Set up for grab distance trigger
	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
	TriggerSphere->InitSphereRadius(80.0f);
	TriggerSphere->BodyInstance.SetCollisionProfileName("Trigger");
	TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &AMyrkurProjectProjectile::OnOverlapBegin);
	TriggerSphere->SetupAttachment(RootComponent);

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	damageAmmount = 34.0f;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3100.f;
	ProjectileMovement->MaxSpeed = 3100.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 2 seconds by default
	InitialLifeSpan = 2.0f;
}

void AMyrkurProjectProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 80.0f, GetActorLocation());

	}

	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		//Add damage to actor hit
		UGameplayStatics::ApplyPointDamage(OtherActor, damageAmmount, GetActorLocation(), Hit, nullptr, this, BluntDamage);
	}
}

void AMyrkurProjectProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp) 
	{
		print("overlap begin");
	}
}
