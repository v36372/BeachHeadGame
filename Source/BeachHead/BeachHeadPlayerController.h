// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "BeachHeadHUD.h"
#include "BeachHeadGameState.h"
#include "BeachHeadPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BEACHHEAD_API ABeachHeadPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	ABeachHeadPlayerController(const FObjectInitializer& ObjectInitializer);

	/* Flag to respawn or start spectating upon death */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	bool bRespawnImmediately;

	/* Respawn or start spectating after dying */
	virtual void UnFreeze() override;

	UFUNCTION(reliable, server, WithValidation)
	void ServerSuicide();

	void ServerSuicide_Implementation();

	bool ServerSuicide_Validate();

public:

	UFUNCTION(reliable, client)
	void ClientHUDStateChanged(EHUDState NewState);

	void ClientHUDStateChanged_Implementation(EHUDState NewState);

	/* Kill the current pawn */
	UFUNCTION(exec)
	virtual void Suicide();

	/* Start spectating. Should be called only on server */
	void StartSpectating();
	
	
};
