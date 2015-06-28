// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerStart.h"
#include "BeachHeadFlyingPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class BEACHHEAD_API ABeachHeadFlyingPlayerStart : public APlayerStart
{
	GENERATED_BODY()
	
	ABeachHeadFlyingPlayerStart(const class FObjectInitializer& ObjectInitializer);

	/* Is only useable by players - automatically a preferred spawn for players */
	UPROPERTY(EditAnywhere, Category = "PlayerStart")
		bool bFlyingOnly;

public:

	bool GetIsFlyingOnly() { return bFlyingOnly; }
	
	
};
