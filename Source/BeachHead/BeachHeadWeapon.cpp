// Fill out your copyright notice in the Description page of Project Settings.
#include "BeachHead.h"
#include "BeachHeadWeapon.h"
#include "BeachHeadPlayerController.h"

// Sets default values
ABeachHeadWeapon::ABeachHeadWeapon(const class FObjectInitializer& PCIP)
: Super(PCIP)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	
	Mesh = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponMesh3P"));
	Mesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	Mesh->bChartDistanceFactor = true;
	Mesh->bReceivesDecals = true;
	Mesh->CastShadow = true;
	Mesh->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	RootComponent = Mesh;
	PrimaryActorTick.bCanEverTick = true;

	StorageSlot = EInventorySlot::Primary;
	CurrentState = EWeaponState::Idle;
	
	bIsEquipped = false;
	LastFireTime = 0;
	ShotsPerMinute = 60;

	MuzzleAttachPoint = TEXT("MuzzleFlashSocket");

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

void ABeachHeadWeapon::OnEquip()
{
	bPendingEquip = true;
	DetermineWeaponState();
	OnEquipFinished();
}

void ABeachHeadWeapon::OnEquipFinished()
{
	bIsEquipped = true;
	bPendingEquip = false;
	DetermineWeaponState();
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

bool ABeachHeadWeapon::ServerHandleFiring_Validate()
{
	return true;
}


void ABeachHeadWeapon::ServerHandleFiring_Implementation()
{
	HandleFiring();
}

bool ABeachHeadWeapon::ServerStartFire_Validate()
{
	return true;
}

void ABeachHeadWeapon::ServerStartFire_Implementation()
{
	StartFire();
}

void ABeachHeadWeapon::StartFire()
{
	if (Role < ROLE_Authority)
	{
		ServerStartFire();
	}

	if (!bWantsToFire)
	{
		bWantsToFire = true;
		DetermineWeaponState();
	}
}

void ABeachHeadWeapon::DetermineWeaponState()
{
	EWeaponState NewState = EWeaponState::Idle;

	if (bIsEquipped)
	{
		/*if (bPendingReload)
		{
			if (CanReload())
			{
				NewState = EWeaponState::Reloading;
			}
			else
			{
				NewState = CurrentState;
			}
		}
		else*/
		if (bWantsToFire && CanFire())
		{
			NewState = EWeaponState::Firing;
		}
	}
	else if (bPendingEquip)
	{
		NewState = EWeaponState::Equipping;
	}

	SetWeaponState(NewState);
}

void ABeachHeadWeapon::SetWeaponState(EWeaponState NewState)
{
	const EWeaponState PrevState = CurrentState;

	if (PrevState == EWeaponState::Firing && NewState != EWeaponState::Firing)
	{
		OnBurstFinished();
	}

	CurrentState = NewState;

	if (PrevState != EWeaponState::Firing && NewState == EWeaponState::Firing)
	{
		OnBurstStarted();
	}
}

void ABeachHeadWeapon::StopFire()
{
	if (Role < ROLE_Authority)
	{
		ServerStopFire();
	}

	if (bWantsToFire)
	{
		bWantsToFire = false;
		DetermineWeaponState();
	}
}

bool ABeachHeadWeapon::ServerStopFire_Validate()
{
	return true;
}


void ABeachHeadWeapon::ServerStopFire_Implementation()
{
	StopFire();
}

void ABeachHeadWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	/* Setup configuration */
	TimeBetweenShots = 60.0f / ShotsPerMinute;
}

void ABeachHeadWeapon::OnBurstStarted()
{
	// Start firing, can be delayed to satisfy TimeBetweenShots
	UE_LOG(LogTemp, Warning, TEXT("ABeachHeadWepaon::OnBurstStarted"));
	const float GameTime = GetWorld()->GetTimeSeconds();
	if (LastFireTime > 0 && TimeBetweenShots > 0.0f &&
		LastFireTime + TimeBetweenShots > GameTime)
	{
		UE_LOG(LogTemp, Warning, TEXT("ABeachHeadWepaon::SetTimerHandleFiring"));
		GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &ABeachHeadWeapon::HandleFiring, LastFireTime + TimeBetweenShots - GameTime, false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ABeachHeadWepaon::HandleFiring"));
		HandleFiring();
	}
}

void ABeachHeadWeapon::OnBurstFinished()
{
	BurstCounter = 0;

	if (GetNetMode() != NM_DedicatedServer)
	{
		//StopSimulatingWeaponFire();
	}

	GetWorldTimerManager().ClearTimer(TimerHandle_HandleFiring);
	bRefiring = false;
}

void ABeachHeadWeapon::OnRep_BurstCounter()
{
	if (BurstCounter > 0)
	{
		SimulateWeaponFire();
	}
	else
	{
		//StopSimulatingWeaponFire();
	}
}

void ABeachHeadWeapon::HandleFiring()
{
	if (CanFire())
	{
		if (GetNetMode() != NM_DedicatedServer)
		{
			SimulateWeaponFire();
		}

		if (MyPawn && MyPawn->IsLocallyControlled())
		{
			FireWeapon();

			// Update firing FX on remote clients if this is called on server
			//BurstCounter++;
		}
	}
	/*else if (CanReload())
	{
		StartReload();
	}*/
	else if (MyPawn && MyPawn->IsLocallyControlled())
	{
		//if (GetCurrentAmmo() == 0 && !bRefiring)
		//{
		//	PlayWeaponSound(OutOfAmmoSound);
		//}

		///* Reload after firing last round */
		//if (CurrentAmmoInClip <= 0 && CanReload())
		//{
		//	StartReload();
		//}

		/* Stop weapon fire FX, but stay in firing state */
		if (BurstCounter > 0)
		{
			OnBurstFinished();
		}
	}

	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		if (Role < ROLE_Authority)
		{
			ServerHandleFiring();
		}

		/* Retrigger HandleFiring on a delay for automatic weapons */
		bRefiring = (CurrentState == EWeaponState::Firing && TimeBetweenShots > 0.0f);
		if (bRefiring)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &ABeachHeadWeapon::HandleFiring, TimeBetweenShots, false);
		}
	}

	LastFireTime = GetWorld()->GetTimeSeconds();
}

bool ABeachHeadWeapon::CanFire()
{
	return MyPawn->CanFire();
}

FVector ABeachHeadWeapon::GetAdjustedAim() const
{
	ABeachHeadPlayerController* const PC = Instigator ? Cast<ABeachHeadPlayerController>(Instigator->Controller) : nullptr;
	FVector FinalAim = FVector::ZeroVector;

	if (PC)
	{
		FVector CamLoc;
		FRotator CamRot;
		PC->GetPlayerViewPoint(CamLoc, CamRot);

		FinalAim = CamRot.Vector();
	}
	else if (Instigator)
	{
		FinalAim = Instigator->GetBaseAimRotation().Vector();
	}

	return FinalAim;
}


FVector ABeachHeadWeapon::GetCameraDamageStartLocation(const FVector& AimDir) const
{
	ABeachHeadPlayerController* PC = MyPawn ? Cast<ABeachHeadPlayerController>(MyPawn->Controller) : nullptr;
	FVector OutStartTrace = FVector::ZeroVector;

	if (PC)
	{
		FRotator DummyRot;
		PC->GetPlayerViewPoint(OutStartTrace, DummyRot);

		// Adjust trace so there is nothing blocking the ray between the camera and the pawn, and calculate distance from adjusted start
		OutStartTrace = OutStartTrace + AimDir * (FVector::DotProduct((Instigator->GetActorLocation() - OutStartTrace), AimDir));
	}

	return OutStartTrace;
}


FHitResult ABeachHeadWeapon::WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const
{
	FCollisionQueryParams TraceParams(TEXT("WeaponTrace"), true, Instigator);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingle(Hit, TraceFrom, TraceTo, COLLISION_WEAPON, TraceParams);

	return Hit;
}

FVector ABeachHeadWeapon::GetMuzzleLocation() const
{
	return Mesh->GetSocketLocation(MuzzleAttachPoint);
}