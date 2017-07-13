// Fill out your copyright notice in the Description page of Project Settings.

#include "LnB.h"
#include "BaseAIController.h"
#include "BaseEnemy.h"
#include "Player/PlayerCharacter.h"
#include "AIWaypoint.h"

// AI Specific includes
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

ABaseAIController::ABaseAIController()
{
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComponent"));
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

	TargetEnemyKeyName = "TargetEnemy";
	TargetWaypointKeyName = "TargetWaypoint";
	FleeTargetKeyName = "TargetFlee";
}

void ABaseAIController::BeginPlay()
{
	Super::BeginPlay();

}

void ABaseAIController::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);

	ABaseEnemy* NewPawn = Cast<ABaseEnemy>(InPawn);

	if(!NewPawn)
		return;

	EnemyPawn = NewPawn;

	// Setup and run the behavior tree
	if(EnemyPawn->BehaviorTree->BlackboardAsset)
	{
		BlackboardComp->InitializeBlackboard(*EnemyPawn->BehaviorTree->BlackboardAsset);
	}

	// Set out default idle waypoint
	BlackboardComp->SetValueAsObject(TargetWaypointKeyName, EnemyPawn->GetIdleWaypoint());

	BehaviorComp->StartTree(*EnemyPawn->BehaviorTree);
}

void ABaseAIController::UnPossess()
{
	Super::UnPossess();

	BehaviorComp->StopTree();
	EnemyPawn = NULL;
}

bool ABaseAIController::CanAttack()
{
	ABaseEnemy* AIEnemy = Cast<ABaseEnemy>(GetPawn());
	if(!AIEnemy)
		return false;

	// Do we have a target?
	APlayerCharacter* Target = GetTargetEnemy();
	if(!Target)
		return false;

	// Let each enemy handle their specifics
	if(!AIEnemy->CanAttack())
		return false;

	// Is the player within attack range?
	FVector Dist = Target->GetActorLocation() - AIEnemy->GetActorLocation();
	if(Dist.Size() > AIEnemy->GetAttackRange())
		return false;

	// success!
	return true;
}

void ABaseAIController::Attack()
{
	AttackFinished = false;

	ABaseEnemy* AIEnemy = Cast<ABaseEnemy>(GetPawn());
	if(!AIEnemy)
		return;

	// Do we have a target?
	APlayerCharacter* Target = GetTargetEnemy();
	if(!Target)
		return;

	FVector Dir = (Target->GetActorLocation()-AIEnemy->GetActorLocation());
	Dir.Normalize();

	EnemyPawn->SetAimDirection(Dir);
	EnemyPawn->Attack();
}

void ABaseAIController::OnAttackFinish(bool Success)
{
	AttackFinished = true;
}

void ABaseAIController::SetTargetEnemy(APawn* NewTarget)
{
	if(BlackboardComp)
	{
 		BlackboardComp->SetValueAsObject(TargetEnemyKeyName, NewTarget);
	}
}

APlayerCharacter* ABaseAIController::GetTargetEnemy()
{
	if(BlackboardComp)
	{
		APlayerCharacter* Target = Cast<APlayerCharacter>(BlackboardComp->GetValueAsObject(TargetEnemyKeyName));
		if(Target)
			return Target;
	}

	return NULL;
}

bool ABaseAIController::SetNextWaypoint()
{
	if(!EnemyPawn)
		return false;

	// See if this is the first waypoint we need to hit
	if(!NextWaypoint)
	{
		AAIWaypoint* CurrentWaypoint = EnemyPawn->GetIdleWaypoint();

		// No waypoint to set
		if(!CurrentWaypoint || !BlackboardComp)
			return false;
		
		// Set our first waypoint
		BlackboardComp->SetValueAsObject(TargetWaypointKeyName, CurrentWaypoint);
		
		// Set up our nextwaypoint
		AAIWaypoint* NewWaypoint = CurrentWaypoint->GetNextWaypoint();
		if(NewWaypoint)
		{
			NextWaypoint = NewWaypoint;
		}

		return true;
	}
	else
	{
		if(!BlackboardComp)
			return false;

		// Set our next waypoint
		BlackboardComp->SetValueAsObject(TargetWaypointKeyName, NextWaypoint);

		// Set up our nextwaypoint
		AAIWaypoint* NewWaypoint = NextWaypoint->GetNextWaypoint();
		if(NewWaypoint)
		{
			NextWaypoint = NewWaypoint;
		}
		else
		{
			NextWaypoint = NULL;
		}

		return true;
	}

	// No new waypoint
	return false; 
}

bool ABaseAIController::SetFleeTarget()
{
	if(!BlackboardComp || !EnemyPawn)
		return false;

	// Set our flee target
	BlackboardComp->SetValueAsVector(FleeTargetKeyName, EnemyPawn->GetFleeLocation());

	return true;
}