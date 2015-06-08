// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BeachHead.h"
#include "GameFramework/Character.h"
#include "BeachHeadCharacter.generated.h"

UCLASS()
class BEACHHEAD_API ABeachHeadCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	/*--------------------------------PUBLIC METHODS-------------------------------------*/
	// Sets default values for this character's properties
	ABeachHeadCharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	/* OnRep functions can use a parameter to hold the previous value of the variable. Very useful when you need to handle UnEquip etc. */
	UFUNCTION()
	void OnRep_CurrentWeapon(ABeachHeadWeapon* LastWeapon);

	void SetCurrentWeapon(class ABeachHeadWeapon* newWeapon, class ABeachHeadWeapon* LastWeapon = nullptr);

	void SpawnDefaultInventory();

	void AddWeapon(class ABeachHeadWeapon* Weapon);
	/*--------------------------------PUBLIC PROPERTIES-------------------------------------*/
	//UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentWeapon)
	UPROPERTY()
	class ABeachHeadWeapon* CurrentWeapon;

	class ABeachHeadWeapon* PreviousWeapon;

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	TArray<TSubclassOf<class ABeachHeadWeapon>> DefaultInventoryClasses;
	//TArray<ABeachHeadWeapon*> DefaultInventoryClasses;

	UPROPERTY(Transient, Replicated)
	TArray<ABeachHeadWeapon*> Inventory;

	void EquipWeapon(ABeachHeadWeapon* Weapon);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerEquipWeapon(ABeachHeadWeapon* Weapon);

	void ServerEquipWeapon_Implementation(ABeachHeadWeapon* Weapon);

	bool ServerEquipWeapon_Validate(ABeachHeadWeapon* Weapon);

	FRotator GetCameraRotation();

private:
	/*--------------------------------PRIVATE PROPERTIES-------------------------------------*/
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh1P;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* BeachHeadCameraComponent;

	bool bWantsToFire;

	/*--------------------------------PRIVATE METHODS-------------------------------------*/
	/* Mapped to input */
	void OnStartFire();

	/* Mapped to input */
	void OnStopFire();

	void StartWeaponFire();

	void StopWeaponFire();
};
