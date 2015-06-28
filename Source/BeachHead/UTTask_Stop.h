// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UBTTask_Fire.h"
#include "UTTask_Stop.generated.h"

/**
 * 
 */
UCLASS()
class BEACHHEAD_API UUTTask_Stop : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
