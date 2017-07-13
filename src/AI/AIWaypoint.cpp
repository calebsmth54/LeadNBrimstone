//=============================================================================
// Shown here with permission from Waden Kane Game Studios, LLC.
//=============================================================================

#include "LnB.h"
#include "AIWaypoint.h"
#include "Components/ArrowComponent.h"

/** For debug purposes only! */
#if !UE_BUILD_SHIPPING
	 #include "Engine/Engine.h"
#endif

AAIWaypoint::AAIWaypoint()
{

#if !UE_BUILD_SHIPPING
	NextWaypointArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Next Waypoint Arrow"));
	NextWaypointArrow->SetWorldRotation(FRotator::ZeroRotator);
	NextWaypointArrow->SetEditorScale(1.0f);
	NextWaypointArrow->SetupAttachment(RootComponent);
#endif

}

#if !UE_BUILD_SHIPPING
void AAIWaypoint::OnConstruction(const FTransform& Transform)
{
	UpdateDebugArrow();
}

void AAIWaypoint::UpdateDebugArrow()
{
	// If none is set, stop drawing our line
	if(!NextWaypoint)
	{
		NextWaypointArrow->SetWorldRotation(FRotator::ZeroRotator);
	}
	else
	{
		FVector Direction = NextWaypoint->GetActorLocation()-GetActorLocation();
		NextWaypointArrow->SetWorldRotation(Direction.Rotation());
	}
}

// We use this to monitor any changes in the next waypoint in order to update the debug line
void AAIWaypoint::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != nullptr)
		? PropertyChangedEvent.Property->GetFName() : NAME_None;

	// Check if the next waypoint has changed or visibility
	if(PropertyName == GET_MEMBER_NAME_CHECKED(AAIWaypoint, NextWaypoint))
	{
		UpdateDebugArrow();
	}


	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void AAIWaypoint::EditorApplyTranslation(const FVector & DeltaTranslation, bool bAltDown, bool bShiftDown, bool bCtrlDown)
{
	UpdateDebugArrow();

	Super::EditorApplyTranslation(DeltaTranslation, bAltDown, bShiftDown, bCtrlDown);
}
#endif

void AAIWaypoint::SetNextWaypoint(AAIWaypoint* NewWaypoint)
{
	NextWaypoint = NewWaypoint;

#if !UE_BUILD_SHIPPING
	UpdateDebugArrow();
#endif

}
