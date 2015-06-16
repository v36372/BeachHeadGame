// Fill out your copyright notice in the Description page of Project Settings.

#include "BeachHead.h"
#include "BeachHeadAICharacter.h"
#include "BeachHeadAIController.h"

/* AI Specific includes */
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

ABeachHeadAIController::ABeachHeadAIController(const class FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	BehaviorComp = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));
	BlackboardComp = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComp"));

	/* Match with the AI/ZombieBlackboard */
	TargetLocationKeyName = "TargetLocation";
	BotTypeKeyName = "BotType";
	TargetEnemyKeyName = "TargetEnemy";
	SelfActorKeyName = "SelfActor";
	SelfLocationKeyName = "SelfLocation";

	/* Initializes PlayerState so we can assign a team index to AI */
	bWantsPlayerState = true;
}


void ABeachHeadAIController::Possess(class APawn* InPawn)
{
	Super::Possess(InPawn);

	ABeachHeadAICharacter* Bot = Cast<ABeachHeadAICharacter>(InPawn);
	if (Bot)
	{
		if (Bot->BehaviorTree->BlackboardAsset)
		{
			BlackboardComp->InitializeBlackboard(*Bot->BehaviorTree->BlackboardAsset);

			/* Make sure the Blackboard has the type of bot we possessed */
			SetBlackboardBotType(Bot->BotType);
		}

		BehaviorComp->StartTree(*Bot->BehaviorTree);
	}
}

void ABeachHeadAIController::SetBlackboardBotType(EBotBehaviorType NewType)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsEnum(BotTypeKeyName, (uint8)NewType);
	}
}

void ABeachHeadAIController::SetMoveToTarget(APawn* Pawn)
{
	if (BlackboardComp)
	{
		SetTargetEnemy(Pawn);

		if (Pawn)
		{
			BlackboardComp->SetValueAsVector(TargetLocationKeyName, Pawn->GetActorLocation());
		}
	}
}

void ABeachHeadAIController::InitSelf(APawn* Pawn)
{

	if (BlackboardComp)
	{
		SetSelfActor(Pawn);

		if (Pawn)
		{
			BlackboardComp->SetValueAsVector(SelfLocationKeyName, Pawn->GetActorLocation());
		}
	}
}

void ABeachHeadAIController::SetTargetEnemy(APawn* NewTarget)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(TargetEnemyKeyName, NewTarget);
	}
}

void ABeachHeadAIController::SetSelfActor(APawn* NewTarget)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(SelfActorKeyName, NewTarget);
	}
}

ABeachHeadBaseCharacter* ABeachHeadAIController::GetTargetEnemy()
{
	if (BlackboardComp)
	{
		return Cast<ABeachHeadBaseCharacter>(BlackboardComp->GetValueAsObject(TargetEnemyKeyName));
	}

	return nullptr;
}

ABeachHeadBaseCharacter* ABeachHeadAIController::GetSelfActor()
{
	if (BlackboardComp)
	{
		return Cast<ABeachHeadBaseCharacter>(BlackboardComp->GetValueAsObject(SelfActorKeyName));
	}

	return nullptr;
}
