//=============================================================================
// Shown here with permission from Waden Kane Game Studios, LLC.
//=============================================================================

#include "LnB.h"
#include "BTTask_RetrieveWaypoint.h"
#include "AI/BaseAIController.h"
#include "AI/AIWaypoint.h"

UBTTask_RetrieveWaypoint::UBTTask_RetrieveWaypoint()
{
	NodeName = "Find A Waypoint!";
	bNotifyTick = false;

	// Accept only waypoint keys
	CurrentWaypointKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_RetrieveWaypoint, CurrentWaypointKey), AAIWaypoint::StaticClass());
}

EBTNodeResult::Type UBTTask_RetrieveWaypoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABaseAIController* AIController = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
	if(!AIController || !AIController->GetPawn())
		return EBTNodeResult::Failed;

	if(AIController->SetNextWaypoint())
		return EBTNodeResult::Succeeded;

	return EBTNodeResult::Failed;
}




