//=============================================================================
// Shown here with permission from Waden Kane Game Studios, LLC.
//=============================================================================

#pragma once

#include "Weapons/BaseWeapon.h"
#include "MeleeWeapon.generated.h"

/**
 *
 */
UCLASS()
class LNB_API AMeleeWeapon : public ABaseWeapon
{
	GENERATED_BODY()

protected:
	bool bKillBoxActive = false;

	// We use this sphere to test whether the player is in attack range
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LNB|MeleeWeapon")
	UBoxComponent* KillBox;

	FTimerHandle SwingTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LNB|MeleeWeapon")
	float SwingTime;

	FTimerHandle SwingDelayTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LNB|MeleeWeapon")
	float SwingDelay;

	virtual bool CanFire();

	virtual void Fire(ALnBCharacter* WeaponOwner, FVector &Location, FVector &Direction) override;

	UFUNCTION()
	virtual void OnMeleeOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSwingFinished();

	UFUNCTION()
	virtual void OnSwingDelayOver();

public:
	AMeleeWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void FireWeapon() override;
	virtual void PostFire() override;
	virtual void StopFiring() override;

	virtual void OnPickup(APawn* NewOwner) override;
};
