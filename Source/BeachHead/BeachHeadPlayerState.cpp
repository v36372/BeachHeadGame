// Fill out your copyright notice in the Description page of Project Settings.

#include "BeachHead.h"
#include "BeachHeadPlayerState.h"

ABeachHeadPlayerState::ABeachHeadPlayerState(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	/* AI will remain in team 0, players are updated to team 1 through the GameMode::InitNewPlayer */
	TeamNumber = 0;
}

void ABeachHeadPlayerState::Reset()
{
	Super::Reset();

	NumKills = 0;
	NumDeaths = 0;
	Score = 0;
}

void ABeachHeadPlayerState::AddKill()
{
	NumKills++;
}

void ABeachHeadPlayerState::AddDeath()
{
	NumDeaths++;
}

void ABeachHeadPlayerState::ScorePoints(int32 Points)
{
	Score += Points;

	/* Add the score to the global score count */
	ABeachHeadGameState* GS = Cast<ABeachHeadGameState>(GetWorld()->GameState);
	if (GS)
	{
		GS->AddScore(Points);
	}
}


void ABeachHeadPlayerState::SetTeamNumber(int32 NewTeamNumber)
{
	TeamNumber = NewTeamNumber;
}


int32 ABeachHeadPlayerState::GetTeamNumber() const
{
	return TeamNumber;
}

int32 ABeachHeadPlayerState::GetKills() const
{
	return NumKills;
}

int32 ABeachHeadPlayerState::GetDeaths() const
{
	return NumDeaths;
}


float ABeachHeadPlayerState::GetScore() const
{
	return Score;
}


void ABeachHeadPlayerState::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABeachHeadPlayerState, NumKills);
	DOREPLIFETIME(ABeachHeadPlayerState, NumDeaths);
	DOREPLIFETIME(ABeachHeadPlayerState, TeamNumber);
}