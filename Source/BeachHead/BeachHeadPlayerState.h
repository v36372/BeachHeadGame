// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "BeachHeadGameState.h"
#include "BeachHeadPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BEACHHEAD_API ABeachHeadPlayerState : public APlayerState
{
	GENERATED_BODY()
	ABeachHeadPlayerState(const FObjectInitializer& ObjectInitializer);
	UPROPERTY(Transient, Replicated)
		int32 NumKills;

	UPROPERTY(Transient, Replicated)
		int32 NumDeaths;

	/* Team number assigned to player */
	UPROPERTY(Transient, Replicated)
		int32 TeamNumber;

	virtual void Reset() override;

public:
	bool bIsFlyingBot;

	void AddKill();

	void AddDeath();

	void ScorePoints(int32 Points);

	void SetTeamNumber(int32 NewTeamNumber);

	UFUNCTION(BlueprintCallable, Category = "Teams")
		int32 GetTeamNumber() const;

	UFUNCTION(BlueprintCallable, Category = "Score")
		int32 GetKills() const;

	UFUNCTION(BlueprintCallable, Category = "Score")
		int32 GetDeaths() const;

	UFUNCTION(BlueprintCallable, Category = "Score")
		float GetScore() const;
};
