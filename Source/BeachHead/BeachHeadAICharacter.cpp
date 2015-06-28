// Fill out your copyright notice in the Description page of Project Settings.

#include "BeachHead.h"
#include "BeachHeadAIController.h"
#include "BeachHeadBaseCharacter.h"
#include "BeachHeadPlayerState.h"
#include "BeachHeadAICharacter.h"

#include "Perception/PawnSensingComponent.h"

ABeachHeadAICharacter::ABeachHeadAICharacter(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	/* Note: We assign the Controller class in the Blueprint extension of this class
	Because the zombie AIController is a blueprint in content and it's better to avoid content references in code.  */
	/*AIControllerClass = ASZombieAIController::StaticClass();*/

	/* Ignore this channel or it will absorb the trace impacts instead of the skeletal mesh */
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);
	GetCapsuleComponent()->SetCapsuleHalfHeight(96.0f, false);
	GetCapsuleComponent()->SetCapsuleRadius(42.0f);

	/* These values are matched up to the CapsuleComponent above and are used to find navigation paths */
	GetMovementComponent()->NavAgentProps.AgentRadius = 42;
	GetMovementComponent()->NavAgentProps.AgentHeight = 192;

	Health = 75;
	PunchDamage = 10.0f;

	/* By default we will not let the AI patrol, we can override this value per-instance. */
	BotType = EBotBehaviorType::Passive;
	SenseTimeOut = 2.5f;

	bIsSet = false;
	bWantsToFire = false;
	/* Note: Visual Setup is done in the AI/ZombieCharacter Blueprint file */
}


void ABeachHeadAICharacter::BeginPlay()
{
	Super::BeginPlay();

	/* Assign a basic name to identify the bots in the HUD. */
	ABeachHeadPlayerState* PS = Cast<ABeachHeadPlayerState>(PlayerState);
	if (PS)
	{
		PS->SetPlayerName("Bot");
		PS->bIsABot = 1;
	}
}


void ABeachHeadAICharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	/* Check if the last time we sensed a player is beyond the time out value to prevent bot from endlessly following a player. */
	if (!bIsSet)
	{
		ABeachHeadAIController* AIController = Cast<ABeachHeadAIController>(GetController());
		ABeachHeadBaseCharacter* SensedPawn = Cast<ABeachHeadBaseCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn()); /// bull shit code
		if (AIController && SensedPawn->IsAlive())
		{
			AIController->SetMoveToTarget(SensedPawn);
			AIController->SetSelfActor(this);
			SpawnDefaultInventory();
		}
		bIsSet = true;
	}
}


void ABeachHeadAICharacter::OnSeePlayer(APawn* Pawn)
{
	/* Keep track of the time the player was last sensed in order to clear the target */
	LastSeenTime = GetWorld()->GetTimeSeconds();
	bSensedTarget = true;

	ABeachHeadAIController* AIController = Cast<ABeachHeadAIController>(GetController());
	ABeachHeadBaseCharacter* SensedPawn = Cast<ABeachHeadBaseCharacter>(Pawn);
	if (AIController && SensedPawn->IsAlive())
	{
		AIController->SetMoveToTarget(SensedPawn);
	}
}


void ABeachHeadAICharacter::OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume)
{
	bSensedTarget = true;
	LastHeardTime = GetWorld()->GetTimeSeconds();

	ABeachHeadAIController* AIController = Cast<ABeachHeadAIController>(GetController());
	if (AIController)
	{
		AIController->SetMoveToTarget(PawnInstigator);
	}
}


void ABeachHeadAICharacter::PunchHit(AActor* HitActor)
{
	if (HitActor && HitActor != this && IsAlive())
	{
		FPointDamageEvent PointDmg;
		PointDmg.DamageTypeClass = PunchDamageType;
		//PointDmg.HitInfo = Impact;
		//PointDmg.ShotDirection = ShootDir;
		PointDmg.Damage = PunchDamage;

		HitActor->TakeDamage(PointDmg.Damage, PointDmg, GetController(), this);
	}
}


void ABeachHeadAICharacter::SetBotType(EBotBehaviorType NewType)
{
	BotType = NewType;

	ABeachHeadAIController* AIController = Cast<ABeachHeadAIController>(GetController());
	if (AIController)
	{
		AIController->SetBlackboardBotType(NewType);
	}
}

void ABeachHeadAICharacter::OnStartFire()
{
	//GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Red, TEXT("ABeachHeadAICharacter::OnStartFire"));
	StartWeaponFire();
}

void ABeachHeadAICharacter::OnStopFire()
{
	//GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Red, TEXT("ABeachHeadAICharacter::OnStopFire"));
	StopWeaponFire();
}

void ABeachHeadAICharacter::StartWeaponFire()
{
	bWantsToFire = false;
	if (!bWantsToFire)
	{
		bWantsToFire = true;
		if (CurrentWeapon)
		{
			CurrentWeapon->StartFire();
			//GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Red, TEXT("ABeachHeadAICharacter::StartWeaponFire"));
		}
	}
}

void ABeachHeadAICharacter::StopWeaponFire()
{
	if (bWantsToFire)
	{
		bWantsToFire = false;
		if (CurrentWeapon)
		{
			CurrentWeapon->StopFire();
			//GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Red, TEXT("ABeachHeadAICharacter::StopWeaponFire"));
		}
	}
}

void ABeachHeadAICharacter::OnRep_CurrentWeapon(ABeachHeadWeapon* LastWeapon)
{
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
}

void ABeachHeadAICharacter::SetCurrentWeapon(class ABeachHeadWeapon* NewWeapon, class ABeachHeadWeapon* LastWeapon)
{
	CurrentWeapon = NewWeapon;

	if (NewWeapon)
	{
		NewWeapon->SetOwningPawn(this);
		/* Only play equip animation when we already hold an item in hands */
		NewWeapon->OnEquip();
	}
}

void ABeachHeadAICharacter::SpawnDefaultInventory()
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	for (int32 i = 0; i < DefaultInventoryClasses.Num(); i++)
	{
		if (DefaultInventoryClasses[i])
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.bNoCollisionFail = true;
			ABeachHeadWeapon* NewWeapon = GetWorld()->SpawnActor<ABeachHeadWeapon>(DefaultInventoryClasses[i], SpawnInfo);

			AddWeapon(NewWeapon);
		}
	}
}

void ABeachHeadAICharacter::AddWeapon(class ABeachHeadWeapon* Weapon)
{
	if (Weapon && Role == ROLE_Authority)
	{
		Weapon->OnEnterInventory(this);
		Inventory.AddUnique(Weapon);

		// Equip first weapon in inventory
		if (Inventory.Num() > 0 && CurrentWeapon == nullptr)
		{
			EquipWeapon(Inventory[0]);
		}
	}
}

void ABeachHeadAICharacter::EquipWeapon(ABeachHeadWeapon* Weapon)
{
	if (Weapon)
	{
		/* Ignore if trying to equip already equipped weapon */
		if (Weapon == CurrentWeapon)
			return;

		if (Role == ROLE_Authority)
		{
			SetCurrentWeapon(Weapon, CurrentWeapon);
		}
		else
		{
			ServerEquipWeapon(Weapon);
		}
	}
}


bool ABeachHeadAICharacter::ServerEquipWeapon_Validate(ABeachHeadWeapon* Weapon)
{
	return true;
}


void ABeachHeadAICharacter::ServerEquipWeapon_Implementation(ABeachHeadWeapon* Weapon)
{
	EquipWeapon(Weapon);
}

void ABeachHeadAICharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABeachHeadCharacter, CurrentWeapon);
	DOREPLIFETIME(ABeachHeadCharacter, Inventory);
	/* If we did not display the current inventory on the player mesh we could optimize replication by using this replication condition. */
	/* DOREPLIFETIME_CONDITION(ASCharacter, Inventory, COND_OwnerOnly);*/
}