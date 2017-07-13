//=============================================================================
// Shown here with permission from Waden Kane Game Studios, LLC.
//=============================================================================

#pragma once

#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include "BaseProjectile.generated.h"

UCLASS()
class LNB_API ABaseProjectile : public AActor
{
	GENERATED_BODY()

protected:
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "LNB|BaseProjectile")
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleDefaultsOnly, category = "LNB|BaseProjectile")
	UProjectileMovementComponent* MovementComp;

public:
	// Sets default values for this actor's properties
	ABaseProjectile();

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void PostInitializeComponents() override;

	/** Handle Hit */
	//UFUNCTION(BlueprintCallable, category = "LNB|BaseProjectile")
	//void OnProjectileHit( const FHitResult& HitResult );

	UFUNCTION(BlueprintCallable, category = "LNB|BaseProjectile")
	void OnProjectileOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, category = "LNB|BaseProjectile")
	void DestroyProjectile();

	UFUNCTION()
	void SetDamage(float dmg) { Damage = dmg; }

	UFUNCTION()
	void SetCollisionProfile(FName Profile);

	UFUNCTION()
	class ABaseWeapon* GetOwnerWeapon();

	// Visual representation of the projectile
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "LNB|BaseProjectile")
	UStaticMeshComponent* VisualComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LNB|BaseProjectile")
	UStaticMesh* MeshToUse;

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "LNB|BaseProjectile")
	//class UParticleSystemComponent ImpactEffect;
};
