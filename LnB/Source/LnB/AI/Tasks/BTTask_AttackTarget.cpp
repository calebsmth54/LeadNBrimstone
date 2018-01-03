//=============================================================================
// Shown here with permission from Waden Kane Game Studios, LLC.
//=============================================================================
#include "BTTask_AttackTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/BaseAIController.h"

UBTTask_AttackTarget::UBTTask_AttackTarget()
{
	NodeName = "Attack Target Actor!";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_AttackTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABaseAIController* AIController = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
	if(!AIController || !AIController->GetPawn())
		return EBTNodeResult::Failed;

	if(!AIController->CanAttack())
		return EBTNodeResult::Failed;

	AIController->Attack();

	return EBTNodeResult::InProgress;
}

void UBTTask_AttackTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ABaseAIController* AIController = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
	if(AIController == NULL || AIController->GetPawn() == NULL)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}

	if(AIController->IsAttackDone())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

