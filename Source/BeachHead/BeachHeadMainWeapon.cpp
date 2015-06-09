// Fill out your copyright notice in the Description page of Project Settings.

#include "BeachHead.h"
#include "BeachHeadMainWeapon.h"

ABeachHeadMainWeapon::ABeachHeadMainWeapon(const FObjectInitializer& ObjectInitializer)
{
	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(80.f, 15.f, 40.f);
}

void ABeachHeadMainWeapon::FireWeapon()
{
	//GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Red, TEXT("ABeachHeadMainWeapon::FireWeapon"));
	if (ProjectileClass != NULL)
	{
		//GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Green, TEXT("ABeachHeadMainWeapon::FireWeapon12341234213423412341232345234"));
		const FRotator SpawnRotation = MyPawn->GetCameraRotation();
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector SpawnLocation = MyPawn->GetActorLocation() + SpawnRotation.RotateVector(GunOffset);

		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			// spawn the projectile at the muzzle
			World->SpawnActor<ABulletProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
		}
	}
}

