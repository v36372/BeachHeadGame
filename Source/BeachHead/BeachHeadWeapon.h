// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BeachHead.h"
#include "GameFramework/Actor.h"
#include "BeachHeadCharacter.h"
#include "BeachHeadWeapon.generated.h"

UENUM()
enum class EWeaponState
{
	Idle,
	Firing,
	Equipping,
	Reloading
};

//UCLASS(ABSTRACT, Blueprintable)
UCLASS(ABSTRACT, Blueprintable)
class BEACHHEAD_API ABeachHeadWeapon : public AActor
{
	GENERATED_BODY()
	virtual void PostInitializeComponents() override;

	UPROPERTY(EditDefaultsOnly)
	float ShotsPerMinute;

public:	
	// Sets default values for this actor's properties
	ABeachHeadWeapon(const class FObjectInitializer& PCIP);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual void OnEnterInventory(ABeachHeadCharacter* NewOwner);

	FORCEINLINE EInventorySlot GetStorageSlot()
	{
		return StorageSlot;
	}

	/* Set the weapon's owning pawn */
	void SetOwningPawn(ABeachHeadCharacter* NewOwner);

	void OnEquip();
	//virtual void OnUnEquip();

	bool CanFire();
private:
	UFUNCTION()
	void OnRep_BurstCounter();

	UPROPERTY(EditDefaultsOnly)
	FName MuzzleAttachPoint;
public:
	void StartFire();
	void StopFire();

	FVector GetAdjustedAim() const;


	FVector GetCameraDamageStartLocation(const FVector& AimDir) const;


	FHitResult WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const;

	FVector GetMuzzleLocation() const;

private:
	bool bRefiring;

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerHandleFiring();

	void ServerHandleFiring_Implementation();

	bool ServerHandleFiring_Validate();

	EWeaponState CurrentState;

	void SetWeaponState(EWeaponState NewState);

	void DetermineWeaponState();

	virtual void HandleFiring();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerStartFire();

	void ServerStartFire_Implementation();

	bool ServerStartFire_Validate();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerStopFire();

	void ServerStopFire_Implementation();

	bool ServerStopFire_Validate();

	FTimerHandle TimerHandle_HandleFiring;

	bool bWantsToFire;
	bool bIsEquipped;
	bool bPendingEquip;

	void OnEquipFinished();

	void OnBurstStarted();

	void OnBurstFinished();

	float LastFireTime;
	/* Time between shots for repeating fire */
	float TimeBetweenShots;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_BurstCounter)
	int32 BurstCounter;

protected:
	/** weapon mesh: 3rd person view */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_MyPawn)
	class ABeachHeadCharacter* MyPawn;

	virtual void FireWeapon()  PURE_VIRTUAL(ASWeapon::FireWeapon, );
	//virtual void FireWeapon();
	virtual void SimulateWeaponFire();
	UAudioComponent* PlayWeaponSound(USoundCue* SoundToPlay);

	UFUNCTION()
	void OnRep_MyPawn();

	/* The character socket to store this item at. */
	EInventorySlot StorageSlot;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundCue* FireSound;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float FireRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector GunOffset;
};
