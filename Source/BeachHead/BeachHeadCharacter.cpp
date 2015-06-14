// Fill out your copyright notice in the Description page of Project Settings.

#include "BeachHead.h"
#include "BeachHeadCharacter.h"
#include "BeachHeadWeapon.h"


// Sets default values
ABeachHeadCharacter::ABeachHeadCharacter(const class FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	Health = 100;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Create a CameraComponent	
	BeachHeadCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	BeachHeadCameraComponent->AttachParent = GetCapsuleComponent();
	BeachHeadCameraComponent->RelativeLocation = FVector(0, 0, 64.f); // Position the camera
	BeachHeadCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	Mesh1P->AttachParent = BeachHeadCameraComponent;
	Mesh1P->RelativeLocation = FVector(0.f, 0.f, -150.f);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;

	MaxUseDistance = 500;
	UE_LOG(LogTemp, Warning, TEXT("character initialize"));
	//FocusedEnemy = nullptr;
	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P are set in the
	// derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

// Called when the game starts or when spawned
void ABeachHeadCharacter::BeginPlay()
{
	Super::BeginPlay();
	SpawnDefaultInventory();
}

// Called every frame
void ABeachHeadCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

// Called to bind functionality to input
void ABeachHeadCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
	//GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Red, TEXT("set up input"));
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	InputComponent->BindAction("Fire", IE_Pressed, this, &ABeachHeadCharacter::OnStartFire);
	InputComponent->BindAction("Fire", IE_Released, this, &ABeachHeadCharacter::OnStopFire);
}

void ABeachHeadCharacter::OnStartFire()
{
	//GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Red, TEXT("ABeachHeadCharacter::OnStartFire"));
	StartWeaponFire();
}

void ABeachHeadCharacter::OnStopFire()
{
	//GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Red, TEXT("ABeachHeadCharacter::OnStopFire"));
	StopWeaponFire();
}

void ABeachHeadCharacter::StartWeaponFire()
{
	if (!bWantsToFire)
	{
		bWantsToFire = true;
		if (CurrentWeapon)
		{
			CurrentWeapon->StartFire();
			//GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Red, TEXT("ABeachHeadCharacter::StartWeaponFire"));
		}
	}
}

void ABeachHeadCharacter::StopWeaponFire()
{
	if (bWantsToFire)
	{
		bWantsToFire = false;
		if (CurrentWeapon)
		{
			CurrentWeapon->StopFire();
			//GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Red, TEXT("ABeachHeadCharacter::StopWeaponFire"));
		}
	}
}

void ABeachHeadCharacter::OnRep_CurrentWeapon(ABeachHeadWeapon* LastWeapon)
{
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
}

void ABeachHeadCharacter::SetCurrentWeapon(class ABeachHeadWeapon* NewWeapon, class ABeachHeadWeapon* LastWeapon)
{
	CurrentWeapon = NewWeapon;

	if (NewWeapon)
	{
		NewWeapon->SetOwningPawn(this);
		/* Only play equip animation when we already hold an item in hands */
		NewWeapon->OnEquip();
	}
}

void ABeachHeadCharacter::SpawnDefaultInventory()
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	for (int32 i = 0; i < DefaultInventoryClasses.Num(); i++)
	{
		if (DefaultInventoryClasses[i])
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.bNoCollisionFail = true;
			ABeachHeadWeapon* NewWeapon = GetWorld()->SpawnActor<ABeachHeadWeapon>(DefaultInventoryClasses[i], SpawnInfo);

			AddWeapon(NewWeapon);
		}
	}
}

void ABeachHeadCharacter::AddWeapon(class ABeachHeadWeapon* Weapon)
{
	if (Weapon && Role == ROLE_Authority)
	{
		Weapon->OnEnterInventory(this);
		Inventory.AddUnique(Weapon);

		// Equip first weapon in inventory
		if (Inventory.Num() > 0 && CurrentWeapon == nullptr)
		{
			EquipWeapon(Inventory[0]);
		}
	}
}

void ABeachHeadCharacter::EquipWeapon(ABeachHeadWeapon* Weapon)
{
	if (Weapon)
	{
		/* Ignore if trying to equip already equipped weapon */
		if (Weapon == CurrentWeapon)
			return;

		if (Role == ROLE_Authority)
		{
			SetCurrentWeapon(Weapon, CurrentWeapon);
		}
		else
		{
			ServerEquipWeapon(Weapon);
		}
	}
}


bool ABeachHeadCharacter::ServerEquipWeapon_Validate(ABeachHeadWeapon* Weapon)
{
	return true;
}


void ABeachHeadCharacter::ServerEquipWeapon_Implementation(ABeachHeadWeapon* Weapon)
{
	EquipWeapon(Weapon);
}

void ABeachHeadCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABeachHeadCharacter, CurrentWeapon);
	DOREPLIFETIME(ABeachHeadCharacter, Inventory);
	/* If we did not display the current inventory on the player mesh we could optimize replication by using this replication condition. */
	/* DOREPLIFETIME_CONDITION(ASCharacter, Inventory, COND_OwnerOnly);*/
}

FRotator ABeachHeadCharacter::GetCameraRotation()
{
	return BeachHeadCameraComponent->GetComponentRotation();
}

bool ABeachHeadCharacter::CanFire()
{
	return IsAlive();
}
