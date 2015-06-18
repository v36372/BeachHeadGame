// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerStart.h"
#include "BeachHeadPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class BEACHHEAD_API ABeachHeadPlayerStart : public APlayerStart
{
	GENERATED_BODY()
	
	ABeachHeadPlayerStart(const class FObjectInitializer& ObjectInitializer);

	/* Is only useable by players - automatically a preferred spawn for players */
	UPROPERTY(EditAnywhere, Category = "PlayerStart")
		bool bPlayerOnly;

public:

	bool GetIsPlayerOnly() { return bPlayerOnly; }
	
	
};
