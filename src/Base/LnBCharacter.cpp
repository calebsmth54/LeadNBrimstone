//=============================================================================
// Shown here with permission from Waden Kane Game Studios, LLC.
//=============================================================================

#include "LnB.h"
#include "LnBCharacter.h"
#include "Weapons/BaseWeapon.h"

/** For debug purposes only! */
#if !UE_BUILD_SHIPPING
	#include "Engine/Engine.h"
#endif

ALnBCharacter::ALnBCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Face in the direction we are moving..
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->GravityScale = 2.f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MaxFlySpeed = 600.f;
	GetCharacterMovement()->bRequestedMoveUseAcceleration = false; // Enables instaneous movement instead of an accelerated ramp up
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, 1.0f, 0.0f)); // By Default, Constrain to the y/z plane
	GetCharacterMovement()->SetPlaneConstraintEnabled(true);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// LnB Character Properties
	//
	Health = 100.0f;

	AimDirection = GetActorForwardVector(); // By default the player is aiming directly ahead
	//
	// End of LnB Character Properties
}

void ALnBCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Equip any weapon that was set in the editor
	FTransform SpawnTM(GetActorRotation(), GetActorLocation());
	ABaseWeapon* NewWeapon = Cast<ABaseWeapon>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, DefaultWeapon, SpawnTM));
	if(NewWeapon)
	{
		NewWeapon->OnPickup(this);

		UGameplayStatics::FinishSpawningActor(NewWeapon, SpawnTM);

		EquipWeapon(NewWeapon);
	}
}

void ALnBCharacter::Tick(float Delta)
{
	Super::Tick(Delta);

	/*
	// Update Actor Yaw facing with Aim Direction
	bool ActorRotationSynced !FMath::IsNearlyEqual(GetActorRotation().Yaw, GetAimDirection().Rotation().Yaw)
	if()
	{
		FRotator NewActorRotation = GetActorRotation();
		float OldActorYaw = EquippedWeapon->GetActorRotation().Yaw;
		float ActorYawGoal = GetAimDirection().Rotation().Yaw;

		NewActorRotation.Yaw = FMath::FInterpConstantTo(OldActorYaw, ActorYawGoal, Delta, 4.0f);

		SetActorRotation(NewActorRotation);
	}*/
}

void ALnBCharacter::EquipWeapon(ABaseWeapon* NewWeapon)
{
	// If we already have a weapon, we need to get rid of it.
	if(EquippedWeapon)
	{
		UnequipWeapon();
	}

	// Change the current weapon to the one we just picked up
	EquippedWeapon = NewWeapon;

	// Let the weapon handle it's pick up initialization
	EquippedWeapon->OnEquip();

}

void ALnBCharacter::UnequipWeapon()
{
	if(EquippedWeapon)
	{
		EquippedWeapon->OnUnequip();
	}
}

FVector ALnBCharacter::GetAimDirection()
{
	return AimDirection;
}

void ALnBCharacter::SetAimDirection(FVector NewAim)
{
	AimDirection = NewAim;

	// Update Actor Yaw Facing
	FRotator NewActorRotation = FRotator::ZeroRotator;
	NewActorRotation.Yaw = AimDirection.Rotation().Yaw;
	SetActorRotation(NewActorRotation);
}

void ALnBCharacter::FireWeapon()
{
	if(!EquippedWeapon)
		return;

	EquippedWeapon->FireWeapon();
}

float ALnBCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	// Apply the damage
	Health -= ActualDamage;

	// Check if our health is below zero, if so, we restart the level
	if(Health <= 0.0f && !IsPendingKill())
	{
		if(!bRagdoll)
		{
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
			GetCapsuleComponent()->SetSimulatePhysics(true);
			SetActorEnableCollision(true);

			if(EquippedWeapon)
				EquippedWeapon->OnDrop();

			USkeletalMeshComponent* Ragdoll = GetMesh();
			if(Ragdoll)
			{
				Ragdoll->SetCollisionProfileName(FName(TEXT("Ragdoll")));
				Ragdoll->SetAllBodiesSimulatePhysics(true);
				Ragdoll->SetSimulatePhysics(true);
				Ragdoll->WakeAllRigidBodies();
				Ragdoll->bBlendPhysics = true;
				bRagdoll = true;
				//Ragdoll->AddImpulseAtLocation(Impulse, HitLocation, Bone);
			}

			GetCharacterMovement()->StopMovementImmediately();
			GetCharacterMovement()->DisableMovement();
			GetCharacterMovement()->SetComponentTickEnabled(false);
		}

		bIsDead = true;
		SetLifeSpan(5.0);
	}

	return ActualDamage;
}

void ALnBCharacter::LifeSpanExpired()
{
	Super::LifeSpanExpired();

	Destroy();
}
