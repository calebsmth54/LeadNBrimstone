// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "LnBPickup.generated.h"

UCLASS(Abstract, Blueprintable)
class LNB_API ALnBPickup : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LNB|LNBPickup")
	UMeshComponent* MeshComponent = NULL;

	// LNB Pickup
	//
	bool PickedUp = false;

	// Spin and oscilation
	// For cosmetic up/down oscilation and rotation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LNB|LNBPickup")
	float ItemSpinRate = 64.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LNB|LNBPickup")
	float ItemBounceRange = 32.0f;

	// Collision sphere for pickups
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LNB|LNBPickup")
	USphereComponent* CollisionComp;
	//
	// END LNB Pickup

public:	
	// Sets default values for this actor's properties
	ALnBPickup();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Rotation and oscilation of the pickup
	virtual void IdleMovement(float DeltaTime);

	// These functions are the primary interface for item interaction
	UFUNCTION(BlueprintCallable, Category = "LNB|LNBPickup")
	virtual void OnPickup(APawn* NewOwner);
	UFUNCTION(BlueprintCallable, Category = "LNB|LNBPickup")
	virtual void OnDrop();
	UFUNCTION(BlueprintCallable, Category = "LNB|LNBPickup")
	virtual void OnEquip() {}
	UFUNCTION(BlueprintCallable, Category = "LNB|LNBPickup")
	virtual void OnUnequip() {}

	UFUNCTION(BlueprintCallable, Category = "LNB|LNBPickup")
	UMeshComponent* GetMeshComponent() { return MeshComponent; }
	UFUNCTION(BlueprintCallable, Category = "LNB|LNBPickup")
	USphereComponent* GetCollisionComponent() { return CollisionComp; }

	UFUNCTION(BlueprintCallable, Category = "LNB|LNBPickup")
	bool IsPickedup() { return PickedUp; }
};
