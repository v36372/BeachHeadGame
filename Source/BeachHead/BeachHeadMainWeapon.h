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

public:
	ABeachHeadMainWeapon(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void FireWeapon();
};
