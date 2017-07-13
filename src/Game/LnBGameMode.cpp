// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "LnB.h"
#include "LnBGameMode.h"

ALnBGameMode::ALnBGameMode()
{
	// set default pawn class to our Blueprinted character
	/*
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/LnBContent/Blueprints/MC_Knight_BP"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// setup the default player controller
	PlayerControllerClass = ALnBPlayerController::StaticClass();
	*/
}
