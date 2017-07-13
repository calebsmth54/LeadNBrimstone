//=============================================================================
// Shown here with permission from Waden Kane Game Studios, LLC.
//=============================================================================

#pragma once

#include "GameFramework/PlayerController.h"
#include "LnBPlayerController.generated.h"

UCLASS()
class LNB_API ALnBPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ALnBPlayerController();

	virtual void Tick(float DeltaTime) override;


};
