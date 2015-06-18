// Fill out your copyright notice in the Description page of Project Settings.

#include "BeachHead.h"
#include "BeachHeadAIController.h"
#include "BeachHeadAICharacter.h"
#include "UBTTask_Fire.h"


EBTNodeResult::Type UUBTTask_Fire::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABeachHeadAIController* MyController = Cast<ABeachHeadAIController>(OwnerComp.GetAIOwner());
	if (MyController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	ABeachHeadAICharacter* MyPawn = Cast<ABeachHeadAICharacter>(MyController->GetPawn());
	if (MyPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	MyPawn->OnStartFire();

	return EBTNodeResult::Succeeded;
}



