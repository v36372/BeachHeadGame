// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "BeachHeadBaseCharacter.h"
#include "BeachHeadAIController.generated.h"

/**
 * 
 */
UCLASS()
class BEACHHEAD_API ABeachHeadAIController : public AAIController
{
	GENERATED_BODY()
	
	ABeachHeadAIController(const class FObjectInitializer& ObjectInitializer);

	/* Called whenever the controller possesses a character bot */
	virtual void Possess(class APawn* InPawn) override;

	UBehaviorTreeComponent* BehaviorComp;

	UBlackboardComponent* BlackboardComp;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName TargetEnemyKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName TargetLocationKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName SelfLocationKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName SelfActorKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName BotTypeKeyName;

public:

	//ASBotWaypoint* GetWaypoint();

	ABeachHeadBaseCharacter* GetTargetEnemy();
	ABeachHeadBaseCharacter* GetSelfActor();

	void SetMoveToTarget(APawn* Pawn);

	//void SetWaypoint(ASBotWaypoint* NewWaypoint);

	void SetTargetEnemy(APawn* NewTarget);

	void SetBlackboardBotType(EBotBehaviorType NewType);

	void InitSelf(APawn* Pawn);

	void SetSelfActor(APawn* Pawn);

	/** Returns BehaviorComp subobject **/
	FORCEINLINE UBehaviorTreeComponent* GetBehaviorComp() const { return BehaviorComp; }

	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const { return BlackboardComp; }
	
	
};
