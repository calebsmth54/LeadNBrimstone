//=============================================================================
// Shown here with permission from Waden Kane Game Studios, LLC.
//=============================================================================

#pragma once

#include "Engine/TargetPoint.h"
#include "AIWaypoint.generated.h"

/**
 *
 */
UCLASS()
class LNB_API AAIWaypoint : public ATargetPoint
{
	GENERATED_BODY()

#if !UE_BUILD_SHIPPING
	void UpdateDebugArrow();
	class UArrowComponent* NextWaypointArrow = NULL;
#endif

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LNB Waypoint")
	AAIWaypoint* NextWaypoint = NULL;

public:
	AAIWaypoint();

#if !UE_BUILD_SHIPPING
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void EditorApplyTranslation(const FVector & DeltaTranslation, bool bAltDown, bool bShiftDown,bool bCtrlDown) override;
#endif

	UFUNCTION(BlueprintCallable, Category = "LNB Waypoint")
	virtual AAIWaypoint* GetNextWaypoint() { return NextWaypoint; }

	UFUNCTION(BlueprintCallable, Category = "LnBWaypoint")
	virtual void SetNextWaypoint(AAIWaypoint* NewWaypoint);
};
