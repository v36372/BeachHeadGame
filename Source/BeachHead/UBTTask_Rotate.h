// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "UBTTask_Rotate.generated.h"

/**
 * 
 */
UCLASS()
class BEACHHEAD_API UUBTTask_Rotate : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
