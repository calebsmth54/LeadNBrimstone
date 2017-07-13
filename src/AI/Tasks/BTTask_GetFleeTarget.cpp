// Fill out your copyright notice in the Description page of Project Settings.

#include "LnB.h"
#include "BTTask_GetFleeTarget.h"
#include "AI/BaseAIController.h"
#include "AI/BaseEnemy.h"

UBTTask_GetFleeTarget::UBTTask_GetFleeTarget()
{
	NodeName = "Retrieve the Flee Target";
	bNotifyTick = false;

	// accept only actors and vectors
	EnemyTargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_GetFleeTarget, EnemyTargetKey), ABaseEnemy::StaticClass());
	FleeTargetKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_GetFleeTarget, FleeTargetKey));
}

EBTNodeResult::Type UBTTask_GetFleeTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABaseAIController* AIController = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
	if(!AIController)
		return EBTNodeResult::Failed;

	ACharacter* AICharacter = Cast<ACharacter>(AIController->GetPawn());
	if(!AICharacter)
		return EBTNodeResult::Failed;

	ACharacter* EnemyCharacter = Cast<ACharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(EnemyTargetKey.SelectedKeyName));
	if(!EnemyCharacter)
		return EBTNodeResult::Failed;

	if(!FleeTargetKey.IsSet() || FleeTargetKey.IsNone())
		return EBTNodeResult::Failed;

	FVector ChargeVector = FVector::ZeroVector;
	ChargeVector.X = FMath::Sign(AICharacter->GetActorLocation().X - EnemyCharacter->GetActorLocation().X);
	//ChargeVector.X *= AICharacter->GetFleeDistance();
	ChargeVector.Y = EnemyCharacter->GetActorLocation().Y;

	// TODO: Fire trace to prevent character's from running into walls forever
	/*
	FCollisionQueryParams TraceParams(WeaponFireTag, true, Instigator);
	TraceParams.AddIgnoredActor(this); // Ignore traces against the weapon
	TraceParams.AddIgnoredActor(GetOwner()); // And the actor firing it

	FHitResult HitOut = FHitResult(ForceInit);
	if(World->LineTraceSingleByChannel(HitOut, Location, End, WeaponTraceChannel, TraceParams))
	{
	*/

	OwnerComp.GetBlackboardComponent()->SetValueAsVector(FleeTargetKey.SelectedKeyName, ChargeVector);
}


