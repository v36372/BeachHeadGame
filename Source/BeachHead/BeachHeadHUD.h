// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "BeachHeadCharacter.h"
#include "BeachHeadHUD.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EHUDState : uint8
{
	Playing,
	Spectating,
	MatchEnd
};


/**
*
*/
UCLASS()
class BEACHHEAD_API ABeachHeadHUD : public AHUD
{
	GENERATED_BODY()

	ABeachHeadHUD(const FObjectInitializer& ObjectInitializer);

	FCanvasIcon CenterDotIcon;

	/** Main HUD update loop. */
	virtual void DrawHUD() override;

	void DrawCenterDot();

public:

	/* An event hook to call HUD text events to display in the HUD. Blueprint HUD class must implement how to deal with this event. */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUDEvents")
	void MessageReceived(const FString& TextMessage);

	/* Event hook to update HUD state (eg. to determine visibility of widgets) */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUDEvents")
	void OnStateChanged(EHUDState NewState);
};

