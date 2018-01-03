//=============================================================================
// Shown here with permission from Waden Kane Game Studios, LLC.
//=============================================================================

#ifndef __LNB_H__
#define __LNB_H__

#include "EngineMinimal.h"

	// A trace channel for player fired traces (weapon related)
	#define PLAYER_WEAPON_TRACE ECC_GameTraceChannel1

	// A trace channel for enemy fired traces (weapon related)
	#define ENEMY_WEAPON_TRACE ECC_GameTraceChannel2

	// A collision profile for player characters and attachments
	#define PLAYER_CPROFILE FName(TEXT("PlayerCharacter"))
	
	// A collision profile for enemy characters and attachments
	#define ENEMY_CPROFILE FName(TEXT("EnemyCharacter"))

	// A collision profile for player pick ups
	#define PICKUP_CPROFILE FName(TEXT("PlayerPickup"))

	// A collision profile for enemy agro (alert) spheres
	#define AGRO_CPROFILE FName(TEXT("EnemyAgroSphere"))

	// A collision profile for player fired projectiles
	#define PLAYER_PROJECTILE_CPROFILE FName(TEXT("PlayerProjectile"))
	
	// A collision profile for enemy fired projectiles
	#define ENEMY_PROJECTILE_CPROFILE FName(TEXT("EnemyProjectile"))

#endif
