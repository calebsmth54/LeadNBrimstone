//=============================================================================
// Shown here with permission from Waden Kane Game Studios, LLC.
//=============================================================================

#pragma once

#include "Base/LnBCharacter.h"
#include "BaseAIController.h"
#include "BaseEnemy.generated.h"

// Blueprint events
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackFinishEvent);

UCLASS()
class LNB_API ABaseEnemy : public ALnBCharacter
{
	GENERATED_BODY()

	FTimerHandle AttackTimer;

protected:
	// We use this sphere to test whether the player is in attack range
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LNB|BaseEnemy")
	USphereComponent* AgroSphere;

	// This is the range in which the ai is able to attack a target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LNB|BaseEnemy")
	float AttackRange = 150.0f;

	UPROPERTY(BlueprintReadWrite, Category = "LNB|BaseEnemy")
	bool bSensedTarget = false;

	UPROPERTY(BlueprintReadWrite, Category = "LNB|BaseEnemy")
	bool AttackSuccess = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LNB|BaseEnemy")
	class AAIWaypoint* IdleStart = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LNB|BaseEnemy")
	float FleeDistance = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LNB|BaseEnemy")
	float AttackTime = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LNB|BaseEnemy")
	bool BPCanAttack = true;

	UFUNCTION()
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnAgroOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnAgroEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	// Sets default values for this character's properties
	ABaseEnemy();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Attack functionality
	//
	virtual bool CanAttack();
	virtual void Attack();
	virtual void OnAttackFinish();

	virtual float GetAttackRange() { return AttackRange; }

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// Attack Events
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, meta = (DisplayName = "OnAttack"), category = "LNB|BaseEnemy")
	void OnAttackBPEvent();

	UPROPERTY(BlueprintAssignable, category = "LNB|BaseEnemy")
	FOnAttackEvent OnAttackEvent;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, meta = (DisplayName = "OnAttackFinish"), category = "LNB|BaseEnemy")
	void OnAttackFinishBPEvent();

	UPROPERTY(BlueprintAssignable, category = "LNB|BaseEnemy")
	FOnAttackFinishEvent OnAttackFinishEvent;

	//
	// Attack Functionality

	UFUNCTION()
	class AAIWaypoint* GetIdleWaypoint() { return IdleStart; }

	UFUNCTION()
	FVector GetFleeLocation();

	// Behavior tree functionality
	UPROPERTY(EditDefaultsOnly, Category = "LNB|BaseEnemy")
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(BlueprintReadOnly, Category = "LNB|BaseEnemy")
	bool bIsAttacking = false;
};
