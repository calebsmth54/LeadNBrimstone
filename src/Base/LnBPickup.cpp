// Fill out your copyright notice in the Description page of Project Settings.

#include "LnB.h"
#include "LnBPickup.h"


// Sets default values
ALnBPickup::ALnBPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DummyRoot"), false);

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("Item Collision Sphere"));
	CollisionComp->SetupAttachment(RootComponent);
	CollisionComp->InitSphereRadius(40.0f);
	CollisionComp->bGenerateOverlapEvents = true;
	CollisionComp->SetCollisionProfileName(PICKUP_CPROFILE);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

// Called when the game starts or when spawned
void ALnBPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALnBPickup::Tick(float DeltaTime)
{
	Super::Tick( DeltaTime );

	IdleMovement( DeltaTime );
}

void ALnBPickup::IdleMovement(float DeltaTime)
{
	if(PickedUp)
		return;

	// Tracks where we are in the oscilation
	static float BobTime = 0.0f;

	// Rotate the item constantly
	AddActorLocalRotation(FRotator(0.0f, ItemSpinRate*DeltaTime, 0.0f));

	// After attempting this myself, I just stole the code from the intro to ue4 programming tutorial
	// TODO: is this the best way to do this?
	// Add up and down oscilation
	FVector NewLocation = GetActorLocation();
	float DeltaHeight = (FMath::Sin(BobTime + DeltaTime) - FMath::Sin(BobTime));
	NewLocation.Z += DeltaHeight * ItemBounceRange;
	BobTime += DeltaTime;

	SetActorLocation(NewLocation);
}

void ALnBPickup::OnPickup(APawn* NewOwner)
{
	SetOwner(NewOwner);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionComp->Deactivate();

	PickedUp = true;
}

void ALnBPickup::OnDrop()
{
	SetOwner(NULL);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->Activate();

	PickedUp = false;
}