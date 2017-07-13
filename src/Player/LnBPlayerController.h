// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "LnBPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LNB_API ALnBPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ALnBPlayerController();

	virtual void Tick(float DeltaTime) override;
	
	
};
