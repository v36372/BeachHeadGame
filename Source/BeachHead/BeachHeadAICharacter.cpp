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
		PS->bIsABot = true;
	}
	ABeachHeadAIController* AIController = Cast<ABeachHeadAIController>(GetController());
	ABeachHeadBaseCharacter* SensedPawn = Cast<ABeachHeadBaseCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn()); /// bull shit code
	if (AIController && SensedPawn->IsAlive())
	{
		AIController->SetMoveToTarget(SensedPawn);
	}
}


void ABeachHeadAICharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	/* Check if the last time we sensed a player is beyond the time out value to prevent bot from endlessly following a player. */
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

