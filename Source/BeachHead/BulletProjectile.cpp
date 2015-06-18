// Fill out your copyright notice in the Description page of Project Settings.

#include "BeachHead.h"
#include "BeachHeadAICharacter.h"
#include "BulletProjectile.h"


// Sets default values
ABulletProjectile::ABulletProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(15.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ABulletProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 4000.f;
	ProjectileMovement->MaxSpeed = 4000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->AttachParent = CollisionComp;;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
	HitDamage = 26;
}

// Called when the game starts or when spawned
void ABulletProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABulletProjectile::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ABulletProjectile::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("ABulletProjectile::OnHit"));
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
		UE_LOG(LogTemp, Warning, TEXT("ABulletProjectile::OnHit 123123123123"));
		Destroy();
	}

	if (Cast<ABeachHeadAICharacter>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("ABulletProjectile bi ban trung rui"));
		FPointDamageEvent PointDmg;
		PointDmg.DamageTypeClass = DamageType;
		PointDmg.HitInfo = Hit;
		PointDmg.ShotDirection = NormalImpulse;
		PointDmg.Damage = HitDamage;

		Cast<ABeachHeadAICharacter>(OtherActor)->TakeDamage(PointDmg.Damage, PointDmg, Cast<ABeachHeadAICharacter>(OtherActor)->Controller, this);
	}
}

