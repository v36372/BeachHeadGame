// Fill out your copyright notice in the Description page of Project Settings.

#include "BeachHead.h"
#include "BeachHeadPlayerController.h"
#include "BeachHeadHUD.h"

ABeachHeadHUD::ABeachHeadHUD(const class FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	/* You can use the FObjectFinder in C++ to reference content directly in code. Although it's advisable to avoid this and instead assign content through Blueprint child classes. */
	static ConstructorHelpers::FObjectFinder<UTexture2D> HUDCenterDotObj(TEXT("/Game/MobileStarterContent/HUD/T_CenterDot_M.T_CenterDot_M"));
	CenterDotIcon = UCanvas::MakeIcon(HUDCenterDotObj.Object);
}


void ABeachHeadHUD::DrawHUD()
{
	Super::DrawHUD();

	DrawCenterDot();
}


void ABeachHeadHUD::DrawCenterDot()
{
	float CenterX = Canvas->ClipX / 2;
	float CenterY = Canvas->ClipY / 2;

	float CenterDotScale = 0.07f;

	ABeachHeadPlayerController* PCOwner = Cast<ABeachHeadPlayerController>(PlayerOwner);
	if (PCOwner)
	{
		ABeachHeadCharacter* Pawn = Cast<ABeachHeadCharacter>(PCOwner->GetPawn());
		if (Pawn && Pawn->IsAlive())
		{
			Canvas->SetDrawColor(255, 255, 255, 255);
			Canvas->DrawIcon(CenterDotIcon,
				CenterX - CenterDotIcon.UL*CenterDotScale / 2.0f,
				CenterY - CenterDotIcon.VL*CenterDotScale / 2.0f, CenterDotScale);
		}
	}
}


