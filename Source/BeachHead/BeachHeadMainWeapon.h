// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BeachHeadWeapon.h"
#include "BulletProjectile.h"
#include "BeachHeadMainWeapon.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class BEACHHEAD_API ABeachHeadMainWeapon : public ABeachHeadWeapon
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ABulletProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly)
	float WeaponRange;
public:
	ABeachHeadMainWeapon(const class FObjectInitializer& PCIP);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerNotifyHit(const FHitResult Impact, FVector_NetQuantizeNormal ShootDir);

	void ServerNotifyHit_Implementation(const FHitResult Impact, FVector_NetQuantizeNormal ShootDir);

	bool ServerNotifyHit_Validate(const FHitResult Impact, FVector_NetQuantizeNormal ShootDir);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerNotifyMiss(FVector_NetQuantizeNormal ShootDir);

	void ServerNotifyMiss_Implementation(FVector_NetQuantizeNormal ShootDir);

	bool ServerNotifyMiss_Validate(FVector_NetQuantizeNormal ShootDir);

	UPROPERTY(Transient, ReplicatedUsing = OnRep_HitLocation)
	FVector HitImpactNotify;

	UFUNCTION()
	void OnRep_HitLocation();

	void SimulateInstantHit(const FVector& ImpactPoint);

	UPROPERTY(EditDefaultsOnly)
	float AllowedViewDotHitDir;

	UPROPERTY(EditDefaultsOnly)
	float ClientSideHitLeeway;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly)
	float HitDamage;

protected:


	virtual void FireWeapon();

	void DealDamage(const FHitResult& Impact, const FVector& ShootDir);

	bool ShouldDealDamage(AActor* TestActor) const;

	void ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir);

	void ProcessInstantHitConfirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir);
};
