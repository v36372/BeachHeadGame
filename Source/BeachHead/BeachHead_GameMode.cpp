// Fill out your copyright notice in the Description page of Project Settings.

#include "BeachHead.h"
#include "BeachHeadAIController.h"
#include "BeachHeadPlayerController.h"
#include "BeachHeadHUD.h"
#include "BeachHeadGameState.h"
#include "BeachHeadPlayerState.h"
#include "BeachHead_GameMode.h"
#include "BeachHeadPlayerStart.h"

/* Define a log category for error messages */
DEFINE_LOG_CATEGORY_STATIC(LogGameMode, Log, All);


ABeachHead_GameMode::ABeachHead_GameMode(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	/* Assign the class types used by this gamemode */
	PlayerControllerClass = ABeachHeadPlayerController::StaticClass();
	PlayerStateClass = ABeachHeadPlayerState::StaticClass();
	GameStateClass = ABeachHeadGameState::StaticClass();
	//SpectatorClass = ASSpectatorPawn::StaticClass();

	bAllowFriendlyFireDamage = false;
	//bSpawnAtTeamPlayer = true;
	//bSpawnZombiesAtNight = true;

	/* Start the game at 16:00 */
	//TimeOfDayStart = 16 * 60;
	//NightSurvivedScore = 100;

	/* Default team is 1 for players and 0 for enemies */
	PlayerTeamNum = 1;
	UE_LOG(LogTemp, Warning, TEXT("ABeachHead_GameMode::Initialize"));
}


void ABeachHead_GameMode::InitGameState()
{
	Super::InitGameState();
	UE_LOG(LogTemp, Warning, TEXT("ABeachHead_GameMode::InitGameState"));
}



void ABeachHead_GameMode::StartMatch()
{
	if (!HasMatchStarted())
	{
		UE_LOG(LogTemp, Warning, TEXT("ABeachHead_GameMode::StartMatch"));
		GetWorldTimerManager().SetTimer(TimerHandle_BotSpawns, this, &ABeachHead_GameMode::SpawnBotHandler, 5.0f, true);
	}
	UE_LOG(LogTemp, Warning, TEXT("ABeachHead_GameMode::StartMatch not yet"));
	Super::StartMatch();
}


void ABeachHead_GameMode::DefaultTimer()
{
	Super::DefaultTimer();

	/* Immediately start the match while playing in editor */
	//if (GetWorld()->IsPlayInEditor())
	{
		if (GetMatchState() == MatchState::WaitingToStart)
		{
			StartMatch();
		}
	}

	/* Only increment time of day while game is active */
	if (IsMatchInProgress())
	{
		ABeachHeadGameState* MyGameState = Cast<ABeachHeadGameState>(GameState);
		if (MyGameState)
		{
			/* Increment our time of day */
			MyGameState->ElapsedGameMinutes += MyGameState->GetTimeOfDayIncrement();

			/* Determine our state */
			MyGameState->GetAndUpdateIsNight();

			/* Trigger events when night starts or ends */
			bool CurrentIsNight = MyGameState->GetIsNight();
			if (CurrentIsNight != LastIsNight)
			{
				FString MessageText = CurrentIsNight ? "SURVIVE!" : "You Survived! Now prepare for the coming night!";

				ABeachHeadGameState* MyGameState = Cast<ABeachHeadGameState>(GameState);
				if (MyGameState)
				{
					MyGameState->BroadcastGameMessage(MessageText);
				}

				/* The night just ended, respawn all dead players */
				if (!CurrentIsNight)
				{
					/* Respawn spectating players that died during the night */
					for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
					{
						/* Look for all players that are spectating */
						ABeachHeadPlayerController* MyController = Cast<ABeachHeadPlayerController>(*It);
						if (MyController)
						{
							if (MyController->PlayerState->bIsSpectator)
							{
								RestartPlayer(MyController);
								MyController->ClientHUDStateChanged(EHUDState::Playing);
							}
							else
							{
								/* Player still alive, award him some points */
								ABeachHeadCharacter* MyPawn = Cast<ABeachHeadCharacter>(MyController->GetPawn());
								if (MyPawn && MyPawn->IsAlive())
								{
									ABeachHeadPlayerState* PS = Cast<ABeachHeadPlayerState>(MyController->PlayerState);
									if (PS)
									{
										PS->ScorePoints(NightSurvivedScore);
									}
								}
							}
						}
					}
				}

				/* Update bot states */
				if (CurrentIsNight)
				{
					WakeAllBots();
				}
				else
				{
					PassifyAllBots();
				}
			}

			LastIsNight = MyGameState->bIsNight;
		}
	}
}


bool ABeachHead_GameMode::CanDealDamage(class ABeachHeadPlayerState* DamageCauser, class ABeachHeadPlayerState* DamagedPlayer) const
{
	return true;
}


FString ABeachHead_GameMode::InitNewPlayer(class APlayerController* NewPlayerController, const TSharedPtr<FUniqueNetId>& UniqueId, const FString& Options, const FString& Portal)
{
	FString Result = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

	ABeachHeadPlayerState* NewPlayerState = Cast<ABeachHeadPlayerState>(NewPlayerController->PlayerState);
	if (NewPlayerState)
	{
		NewPlayerState->SetTeamNumber(PlayerTeamNum);
	}

	return Result;
}


float ABeachHead_GameMode::ModifyDamage(float Damage, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const
{
	float ActualDamage = Damage;

	ABeachHeadBaseCharacter* DamagedPawn = Cast<ABeachHeadBaseCharacter>(DamagedActor);
	if (DamagedPawn && EventInstigator)
	{
		ABeachHeadPlayerState* DamagedPlayerState = Cast<ABeachHeadPlayerState>(DamagedPawn->PlayerState);
		ABeachHeadPlayerState* InstigatorPlayerState = Cast<ABeachHeadPlayerState>(EventInstigator->PlayerState);

		// Check for friendly fire
		if (!CanDealDamage(InstigatorPlayerState, DamagedPlayerState))
		{
			ActualDamage = 0.f;
		}
	}

	return ActualDamage;
}


void ABeachHead_GameMode::CheckMatchEnd()
{
	
}


void ABeachHead_GameMode::FinishMatch()
{
}

void ABeachHead_GameMode::Killed(AController* Killer, AController* VictimPlayer, APawn* VictimPawn, const UDamageType* DamageType)
{
	
}


bool ABeachHead_GameMode::ShouldSpawnAtStartSpot(AController* Player)
{
	/* Always pick a random location */
	return false;
}


AActor* ABeachHead_GameMode::ChoosePlayerStart(AController* Player)
{
	TArray<APlayerStart*> PreferredSpawns;
	TArray<APlayerStart*> FallbackSpawns;

	for (int32 i = 0; i < PlayerStarts.Num(); i++)
	{
		APlayerStart* TestStart = PlayerStarts[i];
		if (IsSpawnpointAllowed(TestStart, Player))
		{
			if (IsSpawnpointPreferred(TestStart, Player))
			{
				PreferredSpawns.Add(TestStart);
			}
			else
			{
				FallbackSpawns.Add(TestStart);
			}
		}
	}

	APlayerStart* BestStart = nullptr;
	if (PreferredSpawns.Num() > 0)
	{
		BestStart = PreferredSpawns[FMath::RandHelper(PreferredSpawns.Num())];
	}
	else if (FallbackSpawns.Num() > 0)
	{
		BestStart = FallbackSpawns[FMath::RandHelper(FallbackSpawns.Num())];
	}

	return BestStart ? BestStart : Super::ChoosePlayerStart(Player);
}


bool ABeachHead_GameMode::IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Controller)
{
	if (Controller == nullptr || Controller->PlayerState == nullptr)
		return true;

	/* Check for extended playerstart class */
	ABeachHeadPlayerStart* MyPlayerStart = Cast<ABeachHeadPlayerStart>(SpawnPoint);
	if (MyPlayerStart)
	{
		return MyPlayerStart->GetIsPlayerOnly() && !Controller->PlayerState->bIsABot;
	}

	/* Cast failed, Anyone can spawn at the base playerstart class */
	return true;
}


bool ABeachHead_GameMode::IsSpawnpointPreferred(APlayerStart* SpawnPoint, AController* Controller)
{
	if (SpawnPoint)
	{
		/* Iterate all pawns to check for collision overlaps with the spawn point */
		const FVector SpawnLocation = SpawnPoint->GetActorLocation();
		for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++)
		{
			ACharacter* OtherPawn = Cast<ACharacter>(*It);
			if (OtherPawn)
			{
				const float CombinedHeight = (SpawnPoint->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + OtherPawn->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()) * 2.0f;
				const float CombinedWidth = SpawnPoint->GetCapsuleComponent()->GetScaledCapsuleRadius() + OtherPawn->GetCapsuleComponent()->GetScaledCapsuleRadius();
				const FVector OtherLocation = OtherPawn->GetActorLocation();

				// Check if player overlaps the playerstart
				if (FMath::Abs(SpawnLocation.Z - OtherLocation.Z) < CombinedHeight && (SpawnLocation - OtherLocation).Size2D() < CombinedWidth)
				{
					return false;
				}
			}
		}

		/* Check if spawnpoint is exclusive to players */
		ABeachHeadPlayerStart* MyPlayerStart = Cast<ABeachHeadPlayerStart>(SpawnPoint);
		if (MyPlayerStart)
		{
			return MyPlayerStart->GetIsPlayerOnly() && !Controller->PlayerState->bIsABot;
		}
	}

	return false;
}

void ABeachHead_GameMode::SpawnNewBot()
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.bNoCollisionFail = true;

	ABeachHeadAIController* AIC = GetWorld()->SpawnActor<ABeachHeadAIController>(SpawnInfo);
	RestartPlayer(AIC);
}

/* Used by RestartPlayer() to determine the pawn to create and possess when a bot or player spawns */
UClass* ABeachHead_GameMode::GetDefaultPawnClassForController(AController* InController)
{
	if (Cast<ABeachHeadAIController>(InController))
	{
		return BotPawnClass;
	}

	return Super::GetDefaultPawnClassForController(InController);
}


bool ABeachHead_GameMode::CanSpectate(APlayerController* Viewer, APlayerState* ViewTarget)
{
	return true;
}


void ABeachHead_GameMode::PassifyAllBots()
{
	
}


void ABeachHead_GameMode::WakeAllBots()
{
	
}


void ABeachHead_GameMode::SpawnBotHandler()
{
	ABeachHeadGameState* MyGameState = Cast<ABeachHeadGameState>(GameState);
	if (MyGameState)
	{
		/* Only spawn bots during night time */
		if (MyGameState->GetIsNight())
		{
			/* This could be any dynamic number based on difficulty (eg. increasing after having survived a few nights) */

			/* Check number of available pawns (players included) */
			if (GetWorld()->GetNumPawns() < MaxPawns+1)
			{
				SpawnNewBot();
			}
		}
	}
}


void ABeachHead_GameMode::RestartPlayer(class AController* NewPlayer)
{
	/* Fallback to PlayerStart picking if team spawning is disabled or we're trying to spawn a bot. */
	if (!bSpawnAtTeamPlayer || (NewPlayer->PlayerState && NewPlayer->PlayerState->bIsABot))
	{
		Super::RestartPlayer(NewPlayer);
		return;
	}

	/* Look for a live player to spawn next to */
	FVector SpawnOrigin = FVector::ZeroVector;
	FRotator StartRotation = FRotator::ZeroRotator;
	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++)
	{
		ABeachHeadCharacter* MyCharacter = Cast<ABeachHeadCharacter>(*It);
		if (MyCharacter && MyCharacter->IsAlive())
		{
			/* Get the origin of the first player we can find */
			SpawnOrigin = MyCharacter->GetActorLocation();
			StartRotation = MyCharacter->GetActorRotation();
			break;
		}
	}

	/* No player is alive (yet) - spawn using one of the PlayerStarts */
	if (SpawnOrigin == FVector::ZeroVector)
	{
		Super::RestartPlayer(NewPlayer);
		return;
	}

	/* Get a point on the nav mesh near the other player */
	FVector StartLocation = UNavigationSystem::GetRandomPointInRadius(NewPlayer, SpawnOrigin, 250.0f);

	// Try to create a pawn to use of the default class for this player
	if (NewPlayer->GetPawn() == nullptr && GetDefaultPawnClassForController(NewPlayer) != nullptr)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Instigator = Instigator;
		APawn* ResultPawn = GetWorld()->SpawnActor<APawn>(GetDefaultPawnClassForController(NewPlayer), StartLocation, StartRotation, SpawnInfo);
		if (ResultPawn == nullptr)
		{
			UE_LOG(LogGameMode, Warning, TEXT("Couldn't spawn Pawn of type %s at %s"), *GetNameSafe(DefaultPawnClass), &StartLocation);
		}
		NewPlayer->SetPawn(ResultPawn);
	}

	if (NewPlayer->GetPawn() == nullptr)
	{
		NewPlayer->FailedToSpawnPawn();
	}
	else
	{
		NewPlayer->Possess(NewPlayer->GetPawn());

		// If the Pawn is destroyed as part of possession we have to abort
		if (NewPlayer->GetPawn() == nullptr)
		{
			NewPlayer->FailedToSpawnPawn();
		}
		else
		{
			// Set initial control rotation to player start's rotation
			NewPlayer->ClientSetRotation(NewPlayer->GetPawn()->GetActorRotation(), true);

			FRotator NewControllerRot = StartRotation;
			NewControllerRot.Roll = 0.f;
			NewPlayer->SetControlRotation(NewControllerRot);

			SetPlayerDefaults(NewPlayer->GetPawn());
		}
	}
}

