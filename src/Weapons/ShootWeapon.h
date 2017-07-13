//=============================================================================
// Shown here with permission from Waden Kane Game Studios, LLC.
//=============================================================================

#pragma once

#include "Weapons/BaseWeapon.h"
#include "ShootWeapon.generated.h"

UCLASS()
class LNB_API AShootWeapon : public ABaseWeapon
{
	GENERATED_BODY()

protected:
	void FireProjectile(ALnBCharacter* WeaponOwner, FVector &Location, FVector &Direction);
	void FireTrace(ALnBCharacter* WeaponOwner, FVector &Location, FVector &Direction);

	//Fire the projectile/trace implemented in child classes
	virtual void Fire(ALnBCharacter* WeaponOwner, FVector &Location, FVector &Direction) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LNB|ShootWeapon")
	TSubclassOf<class ABaseProjectile>  ProjectileType;

	UPROPERTY(EditDefaultsOnly, BluePrintReadonly, Category = "LNB|ShootWeapon")
	bool EnableDebugTraces = false;
};
