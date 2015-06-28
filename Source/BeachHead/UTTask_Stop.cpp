// Fill out your copyright notice in the Description page of Project Settings.

#include "BeachHead.h"
#include "BeachHeadAIController.h"
#include "BeachHeadFlyingPawn.h"
#include "UTTask_Stop.h"


EBTNodeResult::Type UUTTask_Stop::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABeachHeadAIController* MyController = Cast<ABeachHeadAIController>(OwnerComp.GetAIOwner());
	if (MyController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	ABeachHeadFlyingPawn* MyPawn = Cast<ABeachHeadFlyingPawn>(MyController->GetPawn());
	if (MyPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	MyPawn->Stop();

	return EBTNodeResult::Succeeded;
}
