//=============================================================================
// Shown here with permission from Waden Kane Game Studios, LLC.
//=============================================================================
#include "PlayerCharacter.h"
#include "LnB.h"
#include "Weapons/BaseWeapon.h"
#include "Base/LnBPickup.h"

/** For debug purposes only! */
#if !UE_BUILD_SHIPPING
	 #include "Engine/Engine.h"
#endif

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Collision Component
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnItemBeginOverlap); // Add response for item pickups
	GetCapsuleComponent()->SetCollisionProfileName(PLAYER_CPROFILE);

	// Adjust movement component settings here
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->CrouchedHalfHeight = 55.0f; // Crouching settings
	GetCharacterMovement()->MaxWalkSpeedCrouched = 0.0f; // Right now the player can't move when crouched.
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true; // TODO: is this needed?

	// Create a camera boom attached to the root (capsule)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Rotation of the character should not affect rotation of boom
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->TargetArmLength = 500.f;
	CameraBoom->SocketOffset = FVector(0.f, 0.f, 75.f);
	CameraBoom->RelativeRotation = FRotator(0.f, 90.f, 0.f);

	// Create a camera and attach to boom
	SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	SideViewCameraComponent->bUsePawnControlRotation = false; // We don't want the controller rotating the camera

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	NewAimInput = FVector::ZeroVector;
	bIsPlayer = true;
}

void APlayerCharacter::Tick(float Delta)
{
	Super::Tick(Delta);

	// Update our aiming with the new input
	if(!NewAimInput.IsZero())
	{
		// Update aim and clear input
		SetAimDirection(NewAimInput);
		NewAimInput = FVector::ZeroVector;

		if(EquippedWeapon)
		{
			FRotator NewWeaponRot = FRotator::ZeroRotator;
			float OldWeaponPitch = EquippedWeapon->GetActorRotation().Pitch;
			float WeaponPitchGoal = GetAimDirection().Rotation().Pitch;

			NewWeaponRot.Pitch = WeaponPitchGoal; //FMath::FInterpConstantTo(OldWeaponPitch, WeaponPitchGoal, Delta, 10.0f);
			NewWeaponRot.Yaw = GetActorRotation().Yaw;

			EquippedWeapon->SetActorRotation(NewWeaponRot);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* Input)
{
	// set up gameplay key bindings
	Input->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	Input->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	Input->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	Input->BindAxis("LookUp", this, &APlayerCharacter::LookUp);
	Input->BindAxis("LookRight", this, &APlayerCharacter::LookRight);

	Input->BindAction("Fire", IE_Pressed, this, &APlayerCharacter::StartFiring);
	Input->BindAction("Fire", IE_Released, this, &APlayerCharacter::StopFiring);

	Input->BindAction("ChangeWeapon", IE_Pressed, this, &APlayerCharacter::ChangeWeapon);

	Input->BindAction("Crouch", IE_Pressed, this, &APlayerCharacter::StartCrouch);
	Input->BindAction("Crouch", IE_Released, this, &APlayerCharacter::EndCrouch);
}

void APlayerCharacter::MoveRight(float Val)
{
	// add movement in that direction
	AddMovementInput(FVector(1.f, 0.0f, 0.f), Val);
}

void APlayerCharacter::LookUp(float Val)
{
	NewAimInput.Z = Val;
}

void APlayerCharacter::LookRight(float Val)
{
	NewAimInput.X = Val;
}
void APlayerCharacter::StartFiring()
{
	FireWeapon();
}

void APlayerCharacter::StopFiring()
{
	if(!EquippedWeapon)
		return;

	EquippedWeapon->StopFiring();
}

void APlayerCharacter::ChangeWeapon()
{
	if(!SavedWeapon)
		return;

	ABaseWeapon* TempSwap = SavedWeapon;

	EquippedWeapon->OnUnequip();
	SavedWeapon = EquippedWeapon;

	EquippedWeapon = TempSwap;
	EquippedWeapon->OnEquip();
}

void APlayerCharacter::StartCrouch()
{
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->bWantsToCrouch = true;
}

void APlayerCharacter::EndCrouch()
{
	GetCharacterMovement()->bWantsToCrouch = false;
}

// Input
//////////////////////////////////////////////////////////////////////////

void APlayerCharacter::OnItemBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABaseWeapon* Weapon = Cast<ABaseWeapon>(OtherActor);
	if(Weapon)
	{
		// If either weapon slots are free
		if(!EquippedWeapon || !SavedWeapon)
			EquipWeapon(Weapon);

		return;
	}

	ALnBPickup* Item = Cast<ALnBPickup>(OtherActor);
	if(Item)
	{
		//Item->OnPickup(this);
		return;
	}
}

void APlayerCharacter::EquipWeapon(ABaseWeapon* NewWeapon)
{
	if(EquippedWeapon)
		EquippedWeapon->OnUnequip();

	if(SavedWeapon)
		SavedWeapon->OnDrop();

	SavedWeapon = EquippedWeapon;

	NewWeapon->OnPickup(this);

	EquippedWeapon = NewWeapon;
	EquippedWeapon->OnEquip();
}

void APlayerCharacter::FireWeapon()
{
	Super::FireWeapon();

}

float APlayerCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if(EquippedWeapon)
		EquippedWeapon->OnDrop();

	// Time that the player stares are their ragdolled corpse
	SetLifeSpan(5.0f);

	return ActualDamage;
}

void APlayerCharacter::LifeSpanExpired()
{
	APlayerController* MyController = Cast<APlayerController>(GetController());
	if(MyController)
	{
		// Disconnects controller so that we can handle restarting and player death
		DetachFromControllerPendingDestroy(); // Is this necessary?
		MyController->RestartLevel();
	}

	Super::LifeSpanExpired();
}
