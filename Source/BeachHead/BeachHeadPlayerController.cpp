// Fill out your copyright notice in the Description page of Project Settings.

#include "BeachHead.h"
#include "BeachHeadPlayerController.h"


ABeachHeadPlayerController::ABeachHeadPlayerController(const class FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	/* Assign the class types we wish to use */
	//PlayerCameraManagerClass = ASPlayerCameraManager::StaticClass();

	/* Example - Can be set to true for debugging, generally a value like this would exist in the GameMode instead */
	bRespawnImmediately = false;
}


void ABeachHeadPlayerController::UnFreeze()
{
	Super::UnFreeze();

	// Check if match is ending or has ended.
	ABeachHeadGameState* MyGameState = Cast<ABeachHeadGameState>(GetWorld()->GameState);
	if (MyGameState && MyGameState->HasMatchEnded())
	{
		/* Don't allow spectating or respawns */
		return;
	}

	/* Respawn or spectate */
	if (bRespawnImmediately)
	{
		ServerRestartPlayer();
	}
	else
	{
		StartSpectating();
	}
}


void ABeachHeadPlayerController::StartSpectating()
{
	/* Update the state on server */
	PlayerState->bIsSpectator = true;
	/* Waiting to respawn */
	bPlayerIsWaiting = true;
	ChangeState(NAME_Spectating);
	/* Push the state update to the client */
	ClientGotoState(NAME_Spectating);

	/* Focus on the remaining alive player */
	ViewAPlayer(1);

	/* Update the HUD to show the spectator screen */
	ClientHUDStateChanged(EHUDState::Spectating);
}


void ABeachHeadPlayerController::Suicide()
{
	if (IsInState(NAME_Playing))
	{
		ServerSuicide();
	}
}

void ABeachHeadPlayerController::ServerSuicide_Implementation()
{
	ABeachHeadCharacter* MyPawn = Cast<ABeachHeadCharacter>(GetPawn());
	if (MyPawn && ((GetWorld()->TimeSeconds - MyPawn->CreationTime > 1) || (GetNetMode() == NM_Standalone)))
	{
		MyPawn->Suicide();
	}
}


bool ABeachHeadPlayerController::ServerSuicide_Validate()
{
	return true;
}


void ABeachHeadPlayerController::ClientHUDStateChanged_Implementation(EHUDState NewState)
{
	ABeachHeadHUD* MyHUD = Cast<ABeachHeadHUD>(GetHUD());
	if (MyHUD)
	{
		MyHUD->OnStateChanged(NewState);
	}
}

