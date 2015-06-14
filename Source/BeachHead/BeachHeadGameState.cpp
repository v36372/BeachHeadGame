// Fill out your copyright notice in the Description page of Project Settings.

#include "BeachHead.h"
#include "BeachHeadPlayerController.h"
#include "BeachHeadGameState.h"

ABeachHeadGameState::ABeachHeadGameState(const class FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	/* 1 minute real time is 10 minutes game time */
	TimeScale = 10.0f;
	bIsNight = false;

	SunriseTimeMark = 6.0f;
	SunsetTimeMark = 18.0f;
}


void ABeachHeadGameState::SetTimeOfDay(float NewTimeOfDay)
{
	ElapsedGameMinutes = NewTimeOfDay;
}


bool ABeachHeadGameState::GetIsNight()
{
	return bIsNight;
}


float ABeachHeadGameState::GetTimeOfDayIncrement()
{
	return (GetWorldSettings()->GetEffectiveTimeDilation() * TimeScale);
}


int32 ABeachHeadGameState::GetElapsedDays()
{
	const float MinutesInDay = 24 * 60;
	const float ElapsedDays = ElapsedGameMinutes / MinutesInDay;
	return FMath::FloorToInt(ElapsedDays);
}


int32 ABeachHeadGameState::GetElapsedFullDaysInMinutes()
{
	const int32 MinutesInDay = 24 * 60;
	return GetElapsedDays() * MinutesInDay;
}


bool ABeachHeadGameState::GetAndUpdateIsNight()
{
	const float TimeOfDay = ElapsedGameMinutes - GetElapsedFullDaysInMinutes();
	if (TimeOfDay > (SunriseTimeMark * 60) && TimeOfDay < (SunsetTimeMark * 60))
	{
		bIsNight = false;
	}
	else
	{
		bIsNight = true;
	}

	return bIsNight;
}


int32 ABeachHeadGameState::GetRealSecondsTillSunrise()
{
	float SunRiseMinutes = (SunriseTimeMark * 60);
	const int32 MinutesInDay = 24 * 60;

	float ElapsedTimeToday = GetElapsedMinutesCurrentDay();
	if (ElapsedTimeToday < SunRiseMinutes)
	{
		/* Still early in day cycle, so easy to get remaining time */
		return (SunRiseMinutes - ElapsedTimeToday) / TimeScale;
	}
	else
	{
		/* Sunrise will happen "tomorrow" so we need to add another full day to get remaining time */
		float MaxTimeTillNextSunrise = MinutesInDay + SunRiseMinutes;
		return (MaxTimeTillNextSunrise - ElapsedTimeToday) / TimeScale;
	}
}


int32 ABeachHeadGameState::GetElapsedMinutesCurrentDay()
{
	return ElapsedGameMinutes - GetElapsedFullDaysInMinutes();
}


/* As with Server side functions, NetMulticast functions have a _Implementation body */
void ABeachHeadGameState::BroadcastGameMessage_Implementation(const FString& NewMessage)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		ABeachHeadPlayerController* MyController = Cast<ABeachHeadPlayerController>(*It);
		if (MyController && MyController->IsLocalController())
		{
			ABeachHeadHUD* MyHUD = Cast<ABeachHeadHUD>(MyController->GetHUD());
			if (MyHUD)
			{
				MyHUD->MessageReceived(NewMessage);
			}
		}
	}
}


int32 ABeachHeadGameState::GetTotalScore()
{
	return TotalScore;
}


void ABeachHeadGameState::AddScore(int32 Score)
{
	TotalScore += Score;
}


void ABeachHeadGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABeachHeadGameState, ElapsedGameMinutes);
	DOREPLIFETIME(ABeachHeadGameState, bIsNight);
}
