//=============================================================================
// Shown here with permission from Waden Kane Game Studios, LLC.
//=============================================================================
#include "BaseWeapon.h"
#include "LnB.h"
#include "Base/LnBCharacter.h"

/** For debug purposes only! */
#if !UE_BUILD_SHIPPING
	 #include "Engine/Engine.h"
#endif

ABaseWeapon::ABaseWeapon()
{
	// Visual representation of the mesh in game
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh Component"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->bReceivesDecals = false;
	MeshComponent->CastShadow = false;
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	Cast<USkeletalMeshComponent>(MeshComponent)->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;

	// Give these default values
	AttachSocketName = FName("WeaponAttach");
	MuzzleSocketName = FName("MuzzleOffset");

	// Event Delegates
	FireEvent.AddDynamic(this, &ABaseWeapon::OnFireBP);
	FireHitEvent.AddDynamic(this, &ABaseWeapon::OnFireHitBP);

	WindUpEvent.AddDynamic(this, &ABaseWeapon::OnWindUpBP);
	WindUpFinishedEvent.AddDynamic(this, &ABaseWeapon::OnWindUpFinishedBP);
}

// Because MeshComponent is a UMeshComponent, it cannot be set directly as a uproperty and will not be visible in the editor
// This overrides that behavior so our designers can set the mesh and see it in updated in the editor
#if WITH_EDITOR
void ABaseWeapon::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != nullptr)
		? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if(PropertyName == GET_MEMBER_NAME_CHECKED(ABaseWeapon, MeshToUse))
	{
		if(MeshComponent != nullptr)
		{
			SetSkeletalMesh(MeshToUse);
		}
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Debug display for weapons
	#if !UE_BUILD_SHIPPING
		if( bShowWeaponDebug )
		{
			FString DebugString = "FireRateTimer: " + FString::SanitizeFloat(GetWorldTimerManager().GetTimerRemaining(FireRateTimer));
			GEngine->AddOnScreenDebugMessage(0, -1.0f, FColor::Red, DebugString);

			DebugString = "WindupTimer: " + FString::SanitizeFloat(GetWorldTimerManager().GetTimerRemaining(WindupTimer));
			GEngine->AddOnScreenDebugMessage(1, -1.0f, FColor::Red, DebugString);

			DebugString = "CooldownTimer: " + FString::SanitizeFloat(GetWorldTimerManager().GetTimerRemaining(OverheatTimer));
			GEngine->AddOnScreenDebugMessage(2, -1.0f, FColor::Red, DebugString);

			DebugString = "BurstTimer: " + FString::SanitizeFloat(GetWorldTimerManager().GetTimerRemaining(BurstTimer));
			GEngine->AddOnScreenDebugMessage(3, -1.0f, FColor::Red, DebugString);

			DebugString = "BurstCounter: " + FString::SanitizeFloat(BurstCount) + "/" + FString::SanitizeFloat(BurstCount);
			GEngine->AddOnScreenDebugMessage(4, -1.0f, FColor::Red, DebugString);
		}
	#endif
}

bool ABaseWeapon::CanFire()
{
	// Have we overheated?
	if( bOverheating )
		return false;

	// Are we in the middle of firing?
	if( GetWorldTimerManager().GetTimerRemaining(FireRateTimer) > 0.0f )
		return false;

	// Ready to fire!
	return true;
}

bool ABaseWeapon::PreFire()
{
	if(WindupTime > 0.0f && !bWindupComplete)
	{
		if(!bWindingUp)
			StartWindup();

		return false;
	}

	return true;
}

void ABaseWeapon::FireWeapon()
{
	if(!PreFire())
		return;

	if(!CanFire())
		return;

	ALnBCharacter* WeaponOwner = Cast<ALnBCharacter>(GetOwner());
	if(!WeaponOwner)
		return; // TODO: Add UE_LOG?

	// Begin firing
	bIsFiring = true;

	FVector AimLoc, AimDir;
	GetAdjustedAim(AimLoc, AimDir);

	if(BurstCount <= 0.0f)
	{
		if(MaxSpread > 0.0f)
			ApplyAimSpread(AimDir);

		Fire(WeaponOwner, AimLoc, AimDir);
		PostFire();
	}
	else
	{
		BurstFire(WeaponOwner, AimLoc, AimDir);
	}
}

void ABaseWeapon::BurstFire(ALnBCharacter* WeaponOwner, FVector &AimLoc, FVector &AimDir)
{
	// Are we firing every trace immediately or with a delay?
	if(BurstFireRate <= 0.0f)
	{
		// Fire every trace/projectile immediately
		for(CurrentBurstCount = 0; CurrentBurstCount < BurstCount; CurrentBurstCount++)
		{
			// Get random spread
			if(MaxSpread > 0.0f)
				ApplyAimSpread(AimDir);

			Fire(WeaponOwner, AimLoc, AimDir);
		}
	}

	// Stagger firing with burst delay
	else
	{
		if(CurrentBurstCount < BurstCount)
		{
			if(MaxSpread > 0.0f)
				ApplyAimSpread(AimDir);

			Fire(WeaponOwner, AimLoc, AimDir);
			GetWorldTimerManager().SetTimer(BurstTimer, this, &ABaseWeapon::FireWeapon, BurstFireRate, false);
			CurrentBurstCount++;

			return; // Break flow preventing postfire from being called until the burst is finished
		}
		else
		{
			CurrentBurstCount = 0;
		}
	}

	// Return to a regular firing mode
	GetWorldTimerManager().ClearTimer(BurstTimer);

	// Finish firing
	PostFire();
}

void ABaseWeapon::Fire(ALnBCharacter* WeaponOwner, FVector &Location, FVector &Direction)
{
	// Because this is a parent interface class, each weapon will create its own specific firing behavior here

	// Broadcast to blueprints
	FireEvent.Broadcast();
}

void ABaseWeapon::PostFire()
{
	// This allows the player to hold down the fire button and have the weapon fire continuously
	GetWorldTimerManager().SetTimer(FireRateTimer, this, &ABaseWeapon::FireWeapon, FireRate, true);
	StartOverheat();
}

void ABaseWeapon::GetAdjustedAim(FVector& AimLocation, FVector& AimDirection)
{
	ALnBCharacter* WeaponOwner = Cast<ALnBCharacter>(GetOwner());

	// Get the direction and location of aiming
	AimDirection = WeaponOwner->GetAimDirection();
	AimLocation = GetMuzzleLocation();
}

void ABaseWeapon::ApplyAimSpread(FVector& AimDirection)
{
	// TODO: Find a cleaner way of avoiding the gimbal lock
	float SpreadPitch = FMath::RandRange(-MaxSpread, MaxSpread);
	FQuat SpreadQuat = FQuat(FRotator(SpreadPitch, 0.0f, 0.0f));

	AimDirection = SpreadQuat.RotateVector(AimDirection);
}

void ABaseWeapon::StopFiring()
{
	float TimeLeft = GetWorldTimerManager().GetTimerRemaining(FireRateTimer);
	GetWorldTimerManager().SetTimer(FireRateTimer, TimeLeft, false); // Disables repeating the callback

	EndWindup();
	EndOverheat();

	bIsFiring = false;
}

void ABaseWeapon::StartWindup()
{
	// If there is remaining time on the timer, use that for our new timer
	float WindupRemaining = FMath::Max(0.0f, GetWorldTimerManager().GetTimerRemaining(WindupTimer));
	WindupRemaining = WindupTime - WindupRemaining; // Subtract time left without winding up

	GetWorldTimerManager().SetTimer(WindupTimer, this, &ABaseWeapon::OnWindupComplete, WindupRemaining, false);
	bWindupComplete = false;

	WindUpEvent.Broadcast();
}

void ABaseWeapon::OnWindupComplete()
{
	bWindupComplete = true;
	FireWeapon();

	WindUpFinishedEvent.Broadcast(true);
}

void ABaseWeapon::EndWindup()
{
	// Reset windup time
	// Later on, we'll reuse this remaining time so that the player doesn't have to wait the full wind up time
	float TimeLeft = FMath::Max(0.0f, GetWorldTimerManager().GetTimerRemaining(WindupTimer));
	TimeLeft = WindupTime - TimeLeft;
	GetWorldTimerManager().SetTimer(WindupTimer, TimeLeft, false);

	bWindupComplete = false;
	WindUpFinishedEvent.Broadcast(false);
}

void ABaseWeapon::StartOverheat()
{
	// If we have overheating, but the timer isn't set, set it.
	if(OverheatTime > 0.0f &&
		GetWorldTimerManager().GetTimerRemaining(OverheatTimer) <= 0.0f)
	{
		GetWorldTimerManager().SetTimer(OverheatTimer, this, &ABaseWeapon::OnOverheated, OverheatTime, false);
	}
}

void ABaseWeapon::EndOverheat()
{
	// Start cooling down
	float TimeLeft = FMath::Max(0.0f, GetWorldTimerManager().GetTimerRemaining(OverheatTimer));
	TimeLeft = OverheatTime - TimeLeft;
	GetWorldTimerManager().SetTimer(OverheatTimer, this, &ABaseWeapon::OnCooldown, TimeLeft, false);
}

void ABaseWeapon::OnOverheated()
{
	// Setup the OverheatTimer with the cool down time
	bOverheating = true;
	GetWorldTimerManager().SetTimer(OverheatTimer, this, &ABaseWeapon::OnCooldown, CooldownTime, false);
}

void ABaseWeapon::OnCooldown()
{
	bOverheating = false;
	GetWorldTimerManager().ClearTimer(OverheatTimer);
}

void ABaseWeapon::OnPickup(APawn* NewOwner)
{
	Super::OnPickup(NewOwner);

	ALnBCharacter* MyCharacter = Cast<ALnBCharacter>(NewOwner);

	if(!MyCharacter)
		return;

	// Set the collision profile for projectiles and traces
	if(MyCharacter->IsPlayer())
	{
		WeaponTraceChannel = PLAYER_WEAPON_TRACE;
		ProjectileCProfile = PLAYER_PROJECTILE_CPROFILE;
	}
	else
	{
		WeaponTraceChannel = ENEMY_WEAPON_TRACE;
		ProjectileCProfile = ENEMY_PROJECTILE_CPROFILE;
	}

	AttachToPawn(MyCharacter);

	FireEvent.AddDynamic(MyCharacter, &ALnBCharacter::OnWeaponFire);
	FireHitEvent.AddDynamic(MyCharacter, &ALnBCharacter::OnWeaponHit);
}

void ABaseWeapon::OnDrop()
{
	Super::OnDrop();

	ALnBCharacter* MyCharacter = Cast<ALnBCharacter>(GetOwner());
	if(!MyCharacter)
		return;

	FireEvent.RemoveDynamic(MyCharacter, &ALnBCharacter::OnWeaponFire);

	SetActorHiddenInGame(false);
	DetachFromPawn();

	// TODO: Give Items/Weapons some distance from the player
}

void ABaseWeapon::OnEquip()
{
	SetActorHiddenInGame(false);
}

void ABaseWeapon::OnUnequip()
{
	SetActorHiddenInGame(true);
}

void ABaseWeapon::AttachToPawn(ACharacter* MyCharacter)
{
	if(!MyCharacter)
		return;

	USkeletalMeshComponent* OwnerMesh = MyCharacter->GetMesh();
	if(!OwnerMesh)
		return; // TODO: Add error log


	AttachToComponent(OwnerMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, false), GetSocketAttachName());
	SetActorRotation(FRotator(0.0f, MyCharacter->GetActorRotation().Yaw, 0.0f));
}

void ABaseWeapon::DetachFromPawn()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

USkeletalMeshComponent* ABaseWeapon::GetSkeletalMeshComp()
{
	USkeletalMeshComponent* SkeletalComp = Cast<USkeletalMeshComponent>(MeshComponent);

	if(!SkeletalComp)
	{
		UE_LOG(LogTemp, Error, TEXT("Mesh Component initialized as a non-skeletal mesh component in weapon code!"));
		return NULL;
	}

	return SkeletalComp;
}

USkeletalMesh* ABaseWeapon::GetSkeletalMesh()
{
	USkeletalMeshComponent* SkeletalComp = GetSkeletalMeshComp();

	if(!SkeletalComp)
		return NULL;

	return SkeletalComp->SkeletalMesh;
}

void ABaseWeapon::SetSkeletalMesh(USkeletalMesh* NewMesh)
{
	USkeletalMeshComponent* SkeletalComp = GetSkeletalMeshComp();

	if(!SkeletalComp)
		return;

	SkeletalComp->SkeletalMesh = NewMesh;
}

FVector ABaseWeapon::GetMuzzleLocation()
{
	USkeletalMeshComponent* SkeletalComp = Cast<USkeletalMeshComponent>(MeshComponent);

	if(!SkeletalComp)
		return FVector::ZeroVector;

	FVector Location = SkeletalComp->GetSocketLocation(MuzzleSocketName);
	Location.Y = 0.0f; // Important to set so that the bullets travel along the playing plane

	return Location;
}
