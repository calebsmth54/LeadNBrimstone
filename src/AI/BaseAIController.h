//=============================================================================
// Shown here with permission from Waden Kane Game Studios, LLC.
//=============================================================================

#pragma once

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "BaseAIController.generated.h"

UCLASS()
class LNB_API ABaseAIController : public AAIController
{
	GENERATED_BODY()

	UBehaviorTreeComponent* BehaviorComp;
	UBlackboardComponent* BlackboardComp;

	UPROPERTY(EditDefaultsOnly, Category = "LNB AIController")
	FName TargetEnemyKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "LNB AIController")
	FName TargetWaypointKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "LNB AIController")
	FName FleeTargetKeyName;

	bool AttackFinished = false;

	class ABaseEnemy* EnemyPawn;

	class AAIWaypoint* NextWaypoint = NULL;

public:
	ABaseAIController();

	virtual void BeginPlay() override;

	virtual void Possess(class APawn* InPawn) override;
	virtual void UnPossess() override;

	// Base Enemy interfaces
	bool CanAttack();
	void Attack();
	virtual void OnAttackFinish(bool Success);
	virtual bool IsAttackDone() { return AttackFinished; }

	// BB Key Accessors
	void SetTargetEnemy(APawn* NewTarget);
	class APlayerCharacter* GetTargetEnemy();

	bool SetNextWaypoint();
	bool SetFleeTarget();
};
