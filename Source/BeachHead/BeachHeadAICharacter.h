// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BeachHeadBaseCharacter.h"
#include "BeachHeadWeapon.h"
#include "BeachHeadAICharacter.generated.h"

/**
 * 
 */
UCLASS()
class BEACHHEAD_API ABeachHeadAICharacter : public ABeachHeadBaseCharacter
{
	GENERATED_BODY()
public:

	/* Last time the player was spotted */
	float LastSeenTime;

	/* Last time the player was heard */
	float LastHeardTime;

	/* Time-out value to clear the sensed position of the player. Should be higher than Sense interval in the PawnSense component not never miss sense ticks. */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
		float SenseTimeOut;

	/* Resets after sense time-out to avoid unneccessary clearing of target each tick */
	bool bSensedTarget;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	class UPawnSensingComponent* PawnSensingComp;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

protected:

	/* Triggered by pawn sensing component when a pawn is spotted */
	/* When using functions as delegates they need to be marked with UFUNCTION(). We assign this function to FSeePawnDelegate */
	UFUNCTION()
		void OnSeePlayer(APawn* Pawn);

	UFUNCTION()
		void OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume);

	/* Deal damage to the Actor that was hit by the punch animation */
	UFUNCTION(BlueprintCallable, Category = "Attacking")
		void PunchHit(AActor* HitActor);

	UPROPERTY(EditDefaultsOnly, Category = "Attacking")
		TSubclassOf<UDamageType> PunchDamageType;

	UPROPERTY(EditDefaultsOnly, Category = "Attacking")
		float PunchDamage;

	bool bIsSet;

public:

	ABeachHeadAICharacter(const class FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadWrite, Category = "Attacking")
		bool bIsPunching;

	/* The bot behavior we want this bot to execute, (passive/patrol) by specifying EditAnywhere we can edit this value per-instance when placed on the map. */
	UPROPERTY(EditAnywhere, Category = "AI")
		EBotBehaviorType BotType;

	/* The thinking part of the brain, steers our zombie and makes decisions based on the data we feed it from the Blackboard */
	/* Assigned at the Character level (instead of Controller) so we may use different zombie behaviors while re-using one controller. */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTree* BehaviorTree;

	/* Change default bot type during gameplay */
	void SetBotType(EBotBehaviorType NewType);
	
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	TArray<TSubclassOf<class ABeachHeadWeapon>> DefaultInventoryClasses;

	UPROPERTY(Transient, Replicated)
	TArray<ABeachHeadWeapon*> Inventory;

	void EquipWeapon(ABeachHeadWeapon* Weapon);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerEquipWeapon(ABeachHeadWeapon* Weapon);

	void ServerEquipWeapon_Implementation(ABeachHeadWeapon* Weapon);

	bool ServerEquipWeapon_Validate(ABeachHeadWeapon* Weapon);


	/* OnRep functions can use a parameter to hold the previous value of the variable. Very useful when you need to handle UnEquip etc. */
	UFUNCTION()
	void OnRep_CurrentWeapon(ABeachHeadWeapon* LastWeapon);

	void SetCurrentWeapon(class ABeachHeadWeapon* newWeapon, class ABeachHeadWeapon* LastWeapon = nullptr);

	void SpawnDefaultInventory();

	void AddWeapon(class ABeachHeadWeapon* Weapon);
	/*--------------------------------PUBLIC PROPERTIES-------------------------------------*/
	UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentWeapon)
	class ABeachHeadWeapon* CurrentWeapon;

	class ABeachHeadWeapon* PreviousWeapon;

	void OnStartFire();
	bool bWantsToFire;
	/* Mapped to input */
	void OnStopFire();

	void StartWeaponFire() override;

	void StopWeaponFire() override;

};
