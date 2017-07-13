// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Base/LnBPickup.h"
#include "BaseWeapon.generated.h"

class ALnBCharacter;

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	WT_Projectile	UMETA(DisplayName = "ProjectileWeapon"),
	WT_Trace		UMETA(DisplayName = "TraceWeapon"),
	WT_Melee		UMETA(DisplayName = "MeleeWeapon")
};

// Blueprint events
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFireEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFireHitEvent, FHitResult, HitResult);


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWindUpEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWindUpFinishedEvent, bool, bWindUpSuccess);

UCLASS(Abstract, Blueprintable)
class LNB_API ABaseWeapon : public ALnBPickup
{
	GENERATED_BODY()

	FName AttachSocketName;
	FName MuzzleSocketName;

protected:
	
	// This function handles the actual firing of a traceline, projectile or melee killbox
	virtual void Fire(ALnBCharacter* WeaponOwner, FVector &Location, FVector &Direction);

	// Called before each firing event. *IMPORTANT* Only called once before each burst
	virtual bool PreFire();

	// Handles burst firing
	virtual void BurstFire(ALnBCharacter* WeaponOwner, FVector &Location, FVector &Direction);
	
	// Called after each firing event. *IMPORTANT* Only called once after each burst
	virtual void PostFire();

	// Set based on whether the owner of the weapon is an enemy or a player character
	ECollisionChannel WeaponTraceChannel;

	// Set based on whether the owner of the weapon is an enemy or a player character
	FName ProjectileCProfile;

	// This is so that a designer can set the mesh in the editor and immediately see it in the level
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "LNB|BaseWeapon")
	USkeletalMesh* MeshToUse;

	// Damage each projectile/traceline/melee collision does
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LNB|BaseWeapon")
	float Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LNB|BaseWeapon")
	EWeaponType WeaponType = EWeaponType::WT_Trace;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LNB|BaseWeapon")
	bool bShowWeaponDebug = false;

	// Fire Rate Properties
	//
	// A timer that controls the firing rate
	FTimerHandle FireRateTimer;
	
	// Controls how quickly the weapon can refire
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LNB|BaseWeapon")
	float FireRate;
	//
	// End Fire Rate Properties

	// Weapon States
	//
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (DisplayName = "IsFiring?"), Category = "LNB|BaseWeapon")
	bool bIsFiring = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (DisplayName = "IsWindingUp?"), Category = "LNB|BaseWeapon")
	bool bWindingUp = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (DisplayName = "IsOverheating?"), Category = "LNB|BaseWeapon")
	bool bOverheating = false;
	//
	// End Weapon States


	// Spread Properties
	//
	// Controls the max angle of spread between +value and -value
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LNB|BaseWeapon", Meta = (ClampMin = "0.0", ClampMax = "90.0", UIMin = "0.0", UIMax = "90.0"))
	float MaxSpread = 0.0f;

	// TODO: Add a Blueprint version that can override this one
	// Applies spread based on spread property
	virtual void ApplyAimSpread(FVector& AimDirection);
	//
	// End Spread Properties

	// Windup Properties
	//
	FTimerHandle WindupTimer;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (DisplayName = "IsWindupComplete?"), Category = "LNB|BaseWeapon")
	bool bWindupComplete = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LNB|BaseWeapon", Meta = (ClampMin = "0.0", UIMin = "0.0"))
	float WindupTime = 0.0f;
	//
	// End Windup Properties

	// Overheating Properties
	// 
	FTimerHandle OverheatTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LNB|BaseWeapon", Meta = (ClampMin = "0.0", UIMin = "0.0"))
	float OverheatTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LNB|BaseWeapon", Meta = (ClampMin = "0.0", UIMin = "0.0"))
	float CooldownTime = 0.0f;
	//
	// End Overheating Properties

	// Burst Fire Properties 
	//
	int CurrentBurstCount = 0;
	FTimerHandle BurstTimer;

	// Controls the time in between each shot in a burst
	// NOTE: FireRate controls the fire rate BETWEEN bursts
	// 0 for instaneous burst, like a shotgun
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LNB|BaseWeapon", Meta = (ClampMin = "0.0", UIMin = "0.0"))
	float BurstFireRate;

	// The number of traces/projectiles that are fired in one burst
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LNB|BaseWeapon")
	int BurstCount;
	//
	// End Burst Fire Properties

	// Functionality for Attaching Weapon to Owner 
	//
	virtual void AttachToPawn(class ACharacter* MyPawn);
	virtual void DetachFromPawn();
	//
	// End Attach to Owner

public:	
	ABaseWeapon();

	#if !UE_BUILD_SHIPPING
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	#endif

	virtual void Tick(float DeltaTime) override;

	// This signals the weapon to fire
	UFUNCTION(BlueprintCallable, Category = "LNB|BaseWeapon")
	virtual void FireWeapon();

	UFUNCTION(BlueprintCallable, Category = "LNB|BaseWeapon")
	virtual void StopFiring();

	UFUNCTION(BlueprintCallable, Category = "LNB|BaseWeapon")
	virtual bool CanFire();

	virtual void OnOverheated();
	virtual void OnCooldown();

	virtual void StartWindup();
	virtual void OnWindupComplete();

	UFUNCTION(BlueprintCallable, Category = "LNB|BaseWeapon")
	virtual void EndWindup();

	virtual void StartOverheat();
	virtual void EndOverheat();

	virtual void OnPickup(APawn* NewOwner) override;
	virtual void OnDrop() override;
	virtual void OnEquip() override;
	virtual void OnUnequip() override;

	// Getters and Setters
	//

	// Returns the time in between each shot, or if in burst fire mode, the time in between each burst
	UFUNCTION(BlueprintCallable, Category = "LNB|BaseWeapon")
	float GetFireRate() { return FireRate; }

	// Sets the time in between each shot
	UFUNCTION(BlueprintCallable, Category = "LNB|BaseWeapon")
	void SetFireRate(float NewFireRate) { FireRate = NewFireRate; }

	// Calculates the new aiming position based on the weapon's muzzle socket location
	UFUNCTION(BlueprintCallable, Category = "LNB|BaseWeapon")
	void GetAdjustedAim(FVector& AimLocation, FVector& AimDirection);

	// Returns the name of the socket the weapon will attach to on the pawn
	UFUNCTION(BlueprintCallable, Category = "LNB|BaseWeapon")
	FName GetSocketAttachName() { return AttachSocketName; }

	UFUNCTION(BlueprintCallable, Category = "LNB|BaseWeapon")
	USkeletalMeshComponent* GetSkeletalMeshComp();

	UFUNCTION(BlueprintCallable, Category = "LNB|BaseWeapon")
	USkeletalMesh* GetSkeletalMesh();

	UFUNCTION(BlueprintCallable, Category = "LNB|BaseWeapon")
	void SetSkeletalMesh(USkeletalMesh* NewMesh);

	// Returns the distance from the owner's center to the muzzle socket location
	UFUNCTION(BlueprintCallable, Category = "LNB|BaseWeapon")
	FVector GetMuzzleLocation();

	UFUNCTION(BlueprintCallable, Category = "LNB|BaseWeapon")
	void SetMuzzleOffset(const FName& NewMuzzleName) { MuzzleSocketName = NewMuzzleName; }

	// TODO: This is a magic number and that is bad.
	// Replace with screen length look up
	float GetTraceDistance() { return 2000.0f; }
	//
	// End Getters and Setters

	// Blueprint Events
	//

	// Called for each shot fired by weapon
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, meta=(DisplayName="OnFire"), category="LNB|BaseWeapon")
	void OnFireBP();

	UPROPERTY(BlueprintAssignable, category="LNB|BaseWeapon")
	FOnFireEvent FireEvent;

	// Called whenever a projectile, traceline or melee killbox successfully hits something
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, meta = (DisplayName = "OnFireHit"), category = "LNB|BaseWeapon")
	void OnFireHitBP(FHitResult HitResult);

	UPROPERTY(BlueprintAssignable, category = "LNB|BaseWeapon")
	FOnFireHitEvent FireHitEvent;

	// Called when a weapon begins winding up
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, meta = (DisplayName = "OnWindUp"), category = "LNB|BaseWeapon")
	void OnWindUpBP();

	UPROPERTY(BlueprintAssignable, category = "LNB|BaseWeapon")
	FWindUpEvent WindUpEvent;

	// Called when a weapon finishes its windup and begins firing
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, meta = (DisplayName = "OnWindUpFinished"), category = "LNB|BaseWeapon")
	void OnWindUpFinishedBP(bool bWindUpSuccess);

	UPROPERTY(BlueprintAssignable, category = "LNB|BaseWeapon")
	FWindUpFinishedEvent WindUpFinishedEvent;
	//
	// End Blueprint Events
};
