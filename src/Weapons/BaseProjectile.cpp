// Fill out your copyright notice in the Description page of Project Settings.

#include "LnB.h"
#include "BaseProjectile.h"
#include "BaseWeapon.h"
#include "Base/LnBCharacter.h"

/** For debug purposes only! */
#if !UE_BUILD_SHIPPING
	 #include "Engine/Engine.h"
#endif

// Sets default values
ABaseProjectile::ABaseProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Setup a sphere for collision detection
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->bTraceComplexOnMove = true;
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->bGenerateOverlapEvents = true;
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABaseProjectile::OnProjectileOverlap);
	RootComponent = CollisionComp;

	// Setup projectile movement
	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	MovementComp->UpdatedComponent = CollisionComp;
	MovementComp->InitialSpeed = 2000.0f;
	MovementComp->MaxSpeed = 2000.0f;
	MovementComp->ProjectileGravityScale = 0.0f;
	
	// Setup projectile visaul component
	VisualComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile_Mesh"));
	VisualComp->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	VisualComp->SetWorldScale3D(FVector(0.25f));
	VisualComp->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	VisualComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	VisualComp->SetupAttachment(RootComponent);
	

	// Kept so that there is a default visual comp to a projectile
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BulletVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Cone.Shape_Cone"));

	if( BulletVisualAsset.Succeeded() )
	{
		VisualComp->SetStaticMesh(BulletVisualAsset.Object);
	}
}

#if WITH_EDITOR

void ABaseProjectile::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != nullptr)
		? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if(PropertyName == GET_MEMBER_NAME_CHECKED(ABaseProjectile, MeshToUse))
	{
		if(VisualComp != nullptr)
		{
			VisualComp->SetStaticMesh(MeshToUse);
		}
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

#endif

void ABaseProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	CollisionComp->MoveIgnoreActors.Add(Instigator); // Do not collide with the actor who fired this
	//MovementComp->OnProjectileStop.AddDynamic(this, &ABaseProjectile::OnProjectileHit);
}

void ABaseProjectile::OnProjectileOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ALnBCharacter* HitActor = Cast<ALnBCharacter>(OtherActor);
	if(HitActor)
	{
		// Apply Damage
		HitActor->TakeDamage(Damage, FPointDamageEvent(), GetInstigatorController(), GetOwner());
	}

	if(GetOwnerWeapon())
		GetOwnerWeapon()->FireHitEvent.Broadcast(SweepResult);

	DestroyProjectile();
}

void ABaseProjectile::DestroyProjectile()
{

	K2_DestroyActor();
}

ABaseWeapon* ABaseProjectile::GetOwnerWeapon() 
{
	ALnBCharacter* WeaponOwner = Cast<ALnBCharacter>(GetOwner());
	if(!WeaponOwner)
		return NULL;

	return WeaponOwner->GetEquippedWeapon();
}

void ABaseProjectile::SetCollisionProfile(FName Profile)
{
	CollisionComp->SetCollisionProfileName(Profile);
}