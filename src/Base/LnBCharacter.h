// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "LnBCharacter.generated.h"

UCLASS(config=Game)
class ALnBCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	bool bIsPlayer = false;
	bool bRagdoll = false;
	bool bIsDead = false;

	/** Character base properties */
	// Bizaare that actors don't have this value by default
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LNB|LNBCharacter")
	float Health;

	// Contains the curretly equipped weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LNB|LNBCharacter")
	class ABaseWeapon* EquippedWeapon;

	// Accessor so that the weapon can be set in Blueprints
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LNB|LNBCharacter")
	TSubclassOf<class ABaseWeapon>  DefaultWeapon;

	// Where the character is looking
	// We use this instead of the eye angles or view angles in the player controller
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LNB|LNBCharacter")
	FVector AimDirection;

public:
	ALnBCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float Delta) override;

	// Base Weapon Interface
	//
	// This prevents the aim direction from being dsync with the character's rotation
	//virtual void UpdateAimYaw();

	virtual void FireWeapon();
	//
	//End Base Weapon Interface

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// Called after life time has elapsed and character is ready to be destroyed
	virtual void LifeSpanExpired() override;

	/** Getter Setter Functions **/
	UFUNCTION(BlueprintCallable, Category = "LNB|LNBCharacter")
	virtual float GetHealth() { return Health; }

	UFUNCTION(BlueprintCallable, Category = "LNB|LNBCharacter")
	virtual void SetHealth(float NewHealth) { Health = NewHealth; }

	UFUNCTION(BlueprintCallable, Category = "LNB|LNBCharacter")
	virtual void EquipWeapon(ABaseWeapon* NewWeapon);

	UFUNCTION(BlueprintCallable, Category = "LNB|LNBCharacter")
	virtual void UnequipWeapon();

	UFUNCTION(BlueprintCallable, Category = "LNB|LNBCharacter")
	virtual FVector GetAimDirection();

	UFUNCTION(BlueprintCallable, Category = "LNB|LNBCharacter")
	virtual void SetAimDirection(FVector NewAim);

	UFUNCTION(BlueprintCallable, Category = "LNB|LNBCharacter")
	virtual ABaseWeapon* GetEquippedWeapon() { return EquippedWeapon; }

	UFUNCTION(BlueprintCallable, Category = "LNB|LNBCharacter")
	bool IsPlayer() { return bIsPlayer; }

	/** End Getter Setter Functions **/

	// Blueprint Events
	//
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, meta = (DisplayName = "OnWeaponFire"), category = "LNB|LNBCharacter")
	void OnWeaponFire();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, meta = (DisplayName = "OnWeaponHit"), category = "LNB|LNBCharacter")
	void OnWeaponHit(FHitResult HitResult);
	//
	// End Blueprint Events
};