// Fill out your copyright notice in the Description page of Project Settings.
#include "BeachHead.h"
#include "BeachHeadWeapon.h"


// Sets default values
ABeachHeadWeapon::ABeachHeadWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	StorageSlot = EInventorySlot::Primary;
}

// Called when the game starts or when spawned
void ABeachHeadWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABeachHeadWeapon::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ABeachHeadWeapon::OnEnterInventory(ABeachHeadCharacter* NewOwner)
{
	SetOwningPawn(NewOwner);
	//AttachMeshToPawn(StorageSlot);
}

void ABeachHeadWeapon::SimulateWeaponFire()
{
	//GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Green, TEXT("ABeachHeadWeapon::SimulateWeaponFire"));
	// try and fire a projectile

	PlayWeaponSound(FireSound);
}

UAudioComponent* ABeachHeadWeapon::PlayWeaponSound(USoundCue* SoundToPlay)
{
	UAudioComponent* AC = nullptr;
	//GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Red, TEXT("ABeachHeadWeapon::PlayWeaponSound123123123123123"));
	if (SoundToPlay && MyPawn)
	{
		GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Yellow, TEXT("ABeachHeadWeapon::PlayWeaponSound"));
		AC = UGameplayStatics::PlaySoundAttached(SoundToPlay, MyPawn->GetRootComponent());
	}

	return AC;
}

void ABeachHeadWeapon::SetOwningPawn(ABeachHeadCharacter* NewOwner)
{
	if (MyPawn != NewOwner)
	{
		Instigator = NewOwner;
		MyPawn = NewOwner;
		// Net owner for RPC calls.
		SetOwner(NewOwner);
	}
}

void ABeachHeadWeapon::OnRep_MyPawn()
{
	if (MyPawn)
	{
		OnEnterInventory(MyPawn);
	}
	else
	{
		//OnLeaveInventory();
	}
}


void ABeachHeadWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABeachHeadWeapon, MyPawn);
}

void ABeachHeadWeapon::StartFire()
{
	SimulateWeaponFire();
	FireWeapon();
}

void ABeachHeadWeapon::StopFire()
{
	
}