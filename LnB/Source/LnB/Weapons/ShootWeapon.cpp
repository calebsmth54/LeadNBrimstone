//=============================================================================
// Shown here with permission from Waden Kane Game Studios, LLC.
//=============================================================================
#include "ShootWeapon.h"
#include "LnB.h"
#include "BaseProjectile.h"
#include "Base/LnBCharacter.h"

/** For debug purposes only! */
#if !UE_BUILD_SHIPPING
	 #include "Engine/Engine.h"
#endif

void AShootWeapon::Fire(ALnBCharacter* WeaponOwner, FVector &Location, FVector &Direction)
{
	Super::Fire(WeaponOwner, Location, Direction);

	if(WeaponType == EWeaponType::WT_Projectile)
		FireProjectile(WeaponOwner, Location, Direction);

	else if(WeaponType == EWeaponType::WT_Trace)
		FireTrace(WeaponOwner, Location, Direction);
}

void AShootWeapon::FireProjectile(ALnBCharacter* WeaponOwner, FVector &Location, FVector &Direction)
{
	// Get a valid world
	UWorld* const World = GetWorld();
	if(!World)
		return;

	// Spawn the bullet into the world!
	FTransform SpawnTM(Direction.Rotation(), Location);
	ABaseProjectile* Projectile = Cast<ABaseProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ProjectileType, SpawnTM));
	if(Projectile)
	{
		if(WeaponOwner)
		{
			Projectile->Instigator = WeaponOwner;
			Projectile->SetOwner(this);
			Projectile->SetDamage(Damage);
			Projectile->SetCollisionProfile(ProjectileCProfile);
		}

		UGameplayStatics::FinishSpawningActor(Projectile, SpawnTM);
	}
}

void AShootWeapon::FireTrace(ALnBCharacter* WeaponOwner, FVector &Location, FVector &Direction)
{
	static FName WeaponFireTag(TEXT("WeaponTrace"));

	UWorld* const World = GetWorld();
	if(!World)
		return;

	FVector End = Location + Direction*GetTraceDistance();

	#if !UE_BUILD_SHIPPING
		if(EnableDebugTraces)
			GetWorld()->DebugDrawTraceTag = WeaponFireTag;
	#endif

	// Fire the trace
	FCollisionQueryParams TraceParams(WeaponFireTag, true, Instigator);
	TraceParams.AddIgnoredActor(this); // Ignore traces against the weapon
	TraceParams.AddIgnoredActor(GetOwner()); // And the actor firing it

	FHitResult HitOut = FHitResult(ForceInit);
	if(World->LineTraceSingleByChannel(HitOut, Location, End, WeaponTraceChannel, TraceParams))
	{
		// If we hit a pawn, lets do some damage to it
		APawn* Victim = Cast<APawn>(HitOut.GetActor());
		if(Victim)
		{
			// Apply damage to them
			Victim->TakeDamage(Damage, FPointDamageEvent(), WeaponOwner->GetController(), WeaponOwner);
		}
	}

	// Broadcast the event
	FireHitEvent.Broadcast(HitOut);
}
