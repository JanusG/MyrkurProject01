// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

class USkeletalMeshComponent;
class UAnimMontage;

UCLASS()
class MYRKURPROJECT_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Main mesh for enemy */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* CMesh;

	/** Ball Mesh : Location at hands of CharMesh */
	UPROPERTY(VisibleAnywhere, Category = Mesh)
	USkeletalMeshComponent* BallMesh;

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AMyrkurProjectProjectile> ProjectileClass;

	/** Animation for throwing the ball */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	UAnimMontage* TrowingAnim;

	/** Animation for the enemy to die */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	UAnimMontage* DyingAnim;

	/** Set damage to Actor */
	void ReceivePointDamage(float Damage, const class UDamageType* DamageType, FVector HitLocation, FVector HitNormal, class UPrimitiveComponent* HitComponent, FName BoneName, FVector ShotFromDirection, class AController* InstigatedBy, AActor* DamageCauser, const FHitResult& HitInfo);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Initialize an attack on the player
	UFUNCTION(BlueprintCallable)
	void Attack();
};
