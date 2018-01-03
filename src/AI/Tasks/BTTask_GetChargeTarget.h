//=============================================================================
// Shown here with permission from Waden Kane Game Studios, LLC.
//=============================================================================

#pragma once

#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_GetChargeTarget.generated.h"

/**
 * 
 */
UCLASS()
class LNB_API UBTTask_GetChargeTarget : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Charge Options")
	FBlackboardKeySelector EnemyTargetKey;

	UPROPERTY(EditAnywhere, Category = "Charge Options")
	FBlackboardKeySelector ChargeTargetKey;

	UBTTask_GetChargeTarget();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
