// Fill out your copyright notice in the Description page of Project Settings.

#include "LnB.h"
#include "MeleeWeapon.h"
#include "Base/LnBCharacter.h"

// Do we need to include this for finding out what type the owner is?
#include "Player/PlayerCharacter.h"

/** For debug purposes only! */
#if !UE_BUILD_SHIPPING
	 #include "Engine/Engine.h"
#endif

AMeleeWeapon::AMeleeWeapon()
{
	SetActorTickEnabled(true);

	WeaponType = EWeaponType::WT_Melee;

	// Melee Box collision
	KillBox = CreateDefaultSubobject<UBoxComponent>(TEXT("MELEE BOX"));
	KillBox->SetupAttachment(RootComponent);
	KillBox->InitBoxExtent(FVector(10.0f, 10.0f, 10.0f));
	KillBox->bGenerateOverlapEvents = true;
	KillBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	KillBox->OnComponentBeginOverlap.AddDynamic(this, &AMeleeWeapon::OnMeleeOverlap);
}

void AMeleeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Debug display for weapons
	#if !UE_BUILD_SHIPPING
		if(bShowWeaponDebug)
		{
			FString DebugString = "SwingTime: " + FString::SanitizeFloat(SwingTime) + " / " + FString::SanitizeFloat(GetWorldTimerManager().GetTimerRemaining(SwingTimer));
			GEngine->AddOnScreenDebugMessage(5, -1.0f, FColor::Red, DebugString);

			if(bIsFiring)
				DebugString = "Melee is firing!";
			else
				DebugString = "Melee is not firing";

			GEngine->AddOnScreenDebugMessage(7, -1.0f, FColor::Red, DebugString);
		}
	#endif
}

bool AMeleeWeapon::CanFire()
{
	if(!Super::CanFire())
		return false;

	// Limit to one swing at a time
	if(GetWorldTimerManager().IsTimerActive(SwingTimer) &&
		GetWorldTimerManager().GetTimerRemaining(SwingTimer) > 0.0f)
		return false;

	return true;
}

void AMeleeWeapon::FireWeapon()
{
	Super::FireWeapon();

}

void AMeleeWeapon::Fire(ALnBCharacter* WeaponOwner, FVector &Location, FVector &Direction)
{
	Super::Fire(WeaponOwner, Location, Direction);
	
	// Set the time for how long 
	GetWorldTimerManager().SetTimer(SwingTimer, this, &AMeleeWeapon::OnSwingFinished, SwingTime, false);
	GetWorldTimerManager().SetTimer(SwingDelayTimer, this, &AMeleeWeapon::OnSwingDelayOver, SwingDelay, false);
}

void AMeleeWeapon::OnSwingFinished()
{
	Super::PostFire();

	bKillBoxActive = false;
}

void AMeleeWeapon::OnSwingDelayOver()
{
	bKillBoxActive = true;
}

void AMeleeWeapon::PostFire() 
{
	// Removed this code so that fire time isn't reset.
}

void AMeleeWeapon::StopFiring()
{
	Super::StopFiring();

	bKillBoxActive = false;
	GetWorldTimerManager().ClearTimer(SwingTimer);
}

void AMeleeWeapon::OnMeleeOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!bKillBoxActive)
		return;

	ALnBCharacter* WeaponOwner = Cast<ALnBCharacter>(GetOwner());
	if(!WeaponOwner)
		return;

	ALnBCharacter* HitActor = Cast<ALnBCharacter>(OtherActor);

	// Apply damage to the player
	HitActor->TakeDamage(Damage, FPointDamageEvent(), WeaponOwner->GetController(), WeaponOwner);
}

void AMeleeWeapon::OnPickup(APawn* NewOwner)
{
	Super::OnPickup(NewOwner);

	KillBox->SetCollisionProfileName(ProjectileCProfile);
	KillBox->MoveIgnoreActors.Add(GetOwner());
}