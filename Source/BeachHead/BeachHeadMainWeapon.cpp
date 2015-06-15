// Fill out your copyright notice in the Description page of Project Settings.

#include "BeachHead.h"
#include "BeachHeadAICharacter.h"
#include "BeachHeadPlayerController.h"
#include "BeachHeadMainWeapon.h"

ABeachHeadMainWeapon::ABeachHeadMainWeapon(const class FObjectInitializer& PCIP)
: Super(PCIP)
{
	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(80.f, 15.f, 40.f);
	WeaponRange = 15000;
	ClientSideHitLeeway = 200.0f;
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

	FVector CamLoc;
	FRotator CamRot;

	if (MyPawn->Controller == nullptr)
		return;

	Cast<ABeachHeadPlayerController>(MyPawn->Controller)->GetPlayerViewPoint(CamLoc, CamRot);
	const FVector TraceStart = CamLoc;
	const FVector Direction = CamRot.Vector();
	const FVector TraceEnd = TraceStart + (Direction *WeaponRange);

	FCollisionQueryParams TraceParams(TEXT("TraceFiringHit"), true, this);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;

	/* Not tracing complex uses the rough collision instead making tiny objects easier to select. */
	TraceParams.bTraceComplex = false;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingle(Hit, TraceStart, TraceEnd, ECC_Visibility, TraceParams);

	//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 1.0f);

	if (Cast<ABeachHeadAICharacter>(Hit.GetActor()))
	{
		UE_LOG(LogTemp, Warning, TEXT("bi ban trung rui"));
	}
}

void ABeachHeadMainWeapon::ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir)
{
	if (MyPawn && MyPawn->IsLocallyControlled() && GetNetMode() == NM_Client)
	{
		// If we are a client and hit something that is controlled by server
		if (Impact.GetActor() && Impact.GetActor()->GetRemoteRole() == ROLE_Authority)
		{
			// Notify the server of our local hit to validate and apply actual hit damage.
			ServerNotifyHit(Impact, ShootDir);
		}
		else if (Impact.GetActor() == nullptr)
		{
			if (Impact.bBlockingHit)
			{
				ServerNotifyHit(Impact, ShootDir);
			}
			else
			{
				ServerNotifyMiss(ShootDir);
			}
		}
	}

	// Process a confirmed hit.
	ProcessInstantHitConfirmed(Impact, Origin, ShootDir);
}

bool ABeachHeadMainWeapon::ShouldDealDamage(AActor* TestActor) const
{
	// If we are an actor on the server, or the local client has authoritative control over actor, we should register damage.
	if (TestActor)
	{
		if (GetNetMode() != NM_Client ||
			TestActor->Role == ROLE_Authority ||
			TestActor->bTearOff)
		{
			return true;
		}
	}

	return false;
}

void ABeachHeadMainWeapon::DealDamage(const FHitResult& Impact, const FVector& ShootDir)
{
	FPointDamageEvent PointDmg;
	PointDmg.DamageTypeClass = DamageType;
	PointDmg.HitInfo = Impact;
	PointDmg.ShotDirection = ShootDir;
	PointDmg.Damage = HitDamage;

	Impact.GetActor()->TakeDamage(PointDmg.Damage, PointDmg, MyPawn->Controller, this);
}

void ABeachHeadMainWeapon::ProcessInstantHitConfirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir)
{
	// Handle damage
	if (ShouldDealDamage(Impact.GetActor()))
	{
		DealDamage(Impact, ShootDir);
	}

	// Play FX on remote clients
	if (Role == ROLE_Authority)
	{
		HitImpactNotify = Impact.ImpactPoint;
	}

	// Play FX locally
	if (GetNetMode() != NM_DedicatedServer)
	{
		SimulateInstantHit(Impact.ImpactPoint);
	}
}

void ABeachHeadMainWeapon::OnRep_HitLocation()
{
	// Played on all remote clients
	SimulateInstantHit(HitImpactNotify);
}

void ABeachHeadMainWeapon::SimulateInstantHit(const FVector& ImpactPoint)
{
	const FVector MuzzleOrigin = GetMuzzleLocation();

	/* Adjust direction based on desired crosshair impact point and muzzle location */
	const FVector AimDir = (ImpactPoint - MuzzleOrigin).GetSafeNormal();

	const FVector EndTrace = MuzzleOrigin + (AimDir * WeaponRange);
	const FHitResult Impact = WeaponTrace(MuzzleOrigin, EndTrace);

	if (Impact.bBlockingHit)
	{
//		SpawnImpactEffects(Impact);
	//	SpawnTrailEffects(Impact.ImpactPoint);
	}
	else
	{
		//SpawnTrailEffects(EndTrace);
	}
}

void ABeachHeadMainWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABeachHeadMainWeapon, HitImpactNotify, COND_SkipOwner);
}

bool ABeachHeadMainWeapon::ServerNotifyHit_Validate(const FHitResult Impact, FVector_NetQuantizeNormal ShootDir)
{
	return true;
}


void ABeachHeadMainWeapon::ServerNotifyHit_Implementation(const FHitResult Impact, FVector_NetQuantizeNormal ShootDir)
{
	// If we have an instigator, calculate the dot between the view and the shot
	if (Instigator && (Impact.GetActor() || Impact.bBlockingHit))
	{
		const FVector Origin = GetMuzzleLocation();
		const FVector ViewDir = (Impact.Location - Origin).GetSafeNormal();

		const float ViewDotHitDir = FVector::DotProduct(Instigator->GetViewRotation().Vector(), ViewDir);
		if (ViewDotHitDir > AllowedViewDotHitDir)
		{
			// TODO: Check for weapon state

			if (Impact.GetActor() == nullptr)
			{
				if (Impact.bBlockingHit)
				{
					ProcessInstantHitConfirmed(Impact, Origin, ShootDir);
				}
			}
			// Assume it told the truth about static things because we don't move and the hit
			// usually doesn't have significant gameplay implications
			else if (Impact.GetActor()->IsRootComponentStatic() || Impact.GetActor()->IsRootComponentStationary())
			{
				ProcessInstantHitConfirmed(Impact, Origin, ShootDir);
			}
			else
			{
				const FBox HitBox = Impact.GetActor()->GetComponentsBoundingBox();

				FVector BoxExtent = 0.5 * (HitBox.Max - HitBox.Min);
				BoxExtent *= ClientSideHitLeeway;

				BoxExtent.X = FMath::Max(20.0f, BoxExtent.X);
				BoxExtent.Y = FMath::Max(20.0f, BoxExtent.Y);
				BoxExtent.Z = FMath::Max(20.0f, BoxExtent.Z);

				const FVector BoxCenter = (HitBox.Min + HitBox.Max) * 0.5;

				// If we are within client tolerance
				if (FMath::Abs(Impact.Location.Z - BoxCenter.Z) < BoxExtent.Z &&
					FMath::Abs(Impact.Location.X - BoxCenter.X) < BoxExtent.X &&
					FMath::Abs(Impact.Location.Y - BoxCenter.Y) < BoxExtent.Y)
				{
					ProcessInstantHitConfirmed(Impact, Origin, ShootDir);
				}
			}
		}
	}

	// TODO: UE_LOG on failures & rejection
}

bool ABeachHeadMainWeapon::ServerNotifyMiss_Validate(FVector_NetQuantizeNormal ShootDir)
{
	return true;
}


void ABeachHeadMainWeapon::ServerNotifyMiss_Implementation(FVector_NetQuantizeNormal ShootDir)
{
	const FVector Origin = GetMuzzleLocation();
	const FVector EndTrace = Origin + (ShootDir * WeaponRange);

	// Play on remote clients
	HitImpactNotify = EndTrace;

	if (GetNetMode() != NM_DedicatedServer)
	{
		//SpawnTrailEffects(EndTrace);
	}
}