//=============================================================================
// Shown here with permission from Waden Kane Game Studios, LLC.
//=============================================================================

#pragma once

#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_GetFleeTarget.generated.h"

/**
 * 
 */
UCLASS()
class LNB_API UBTTask_GetFleeTarget : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Charge Options")
	FBlackboardKeySelector EnemyTargetKey;

	UPROPERTY(EditAnywhere, Category = "Charge Options")
	FBlackboardKeySelector FleeTargetKey;

	UBTTask_GetFleeTarget();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
