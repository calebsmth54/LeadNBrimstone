// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#ifndef __LNB_H__
#define __LNB_H__

#include "EngineMinimal.h"

	#define PLAYER_WEAPON_TRACE ECC_GameTraceChannel1
	#define ENEMY_WEAPON_TRACE ECC_GameTraceChannel2

	#define PLAYER_CPROFILE FName(TEXT("PlayerCharacter"))
	#define ENEMY_CPROFILE FName(TEXT("EnemyCharacter"))
	#define PICKUP_CPROFILE FName(TEXT("PlayerPickup"))
	#define AGRO_CPROFILE FName(TEXT("EnemyAgroSphere"))
	#define PLAYER_PROJECTILE_CPROFILE FName(TEXT("PlayerProjectile"))
	#define ENEMY_PROJECTILE_CPROFILE FName(TEXT("EnemyProjectile"))

#endif
