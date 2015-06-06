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

enum class EInventorySlot
{
	Primary
};

//UCLASS(ABSTRACT, Blueprintable)
UCLASS(ABSTRACT, Blueprintable)
class BEACHHEAD_API ABeachHeadWeapon : public AActor
{
	GENERATED_BODY()
	/* The character socket to store this item at. */
	EInventorySlot StorageSlot;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundCue* FireSound;
public:	
	// Sets default values for this actor's properties
	ABeachHeadWeapon();

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

	void StartFire();
	void StopFire();

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

};
