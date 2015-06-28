// Fill out your copyright notice in the Description page of Project Settings.

#include "BeachHead.h"
#include "BeachHeadAIController.h"
#include "BeachHeadFlyingPawn.h"
#include "UBTTask_Rotate.h"


EBTNodeResult::Type UUBTTask_Rotate::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	if (MyPawn->IsStop())
		return EBTNodeResult::Succeeded;

	FVector TargetLocation = MyController->GetTargetLocation();
	FVector SelfLocation = MyController->GetSelfLocation();

	//const FVector Direction2D = FVector(SelfLocation.X - TargetLocation.X, SelfLocation.Y - TargetLocation.Y, 0);
	FRotator newrot = (TargetLocation - SelfLocation).Rotation();
	//FRotator Rot = FRotationMatrix::MakeFromXZ(FVector(0, SelfLocation.Y - TargetLocation.Y, SelfLocation.Z - TargetLocation.Z)).Rotator();

	MyPawn->SetActorRotation(newrot);

	return EBTNodeResult::Succeeded;
}