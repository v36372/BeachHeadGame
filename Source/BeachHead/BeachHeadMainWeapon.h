// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BeachHeadWeapon.h"
#include "BeachHeadMainWeapon.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class BEACHHEAD_API ABeachHeadMainWeapon : public ABeachHeadWeapon
{
	GENERATED_BODY()

protected:
	virtual void FireWeapon();
};
