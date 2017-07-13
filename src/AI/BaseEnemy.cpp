// Fill out your copyright notice in the Description page of Project Settings.

#include "LnB.h"
#include "AI/BaseEnemy.h"
#include "AI/BaseAIController.h"
#include "Player/PlayerCharacter.h"
#include "AIWaypoint.h"
#include "Weapons/BaseWeapon.h"

/** For debug purposes only! */
#if !UE_BUILD_SHIPPING
	 #include "Engine/Engine.h"
#endif

// Sets default values
ABaseEnemy::ABaseEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Collision properties
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABaseEnemy::OnOverlap);
	GetCapsuleComponent()->SetCollisionProfileName(ENEMY_CPROFILE);

	//GetMesh()->SetCollisionObjectType(COLLISION_ENEMY);

	// Don't rotate when the controller rotates.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Face in the direction we are moving..
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->GravityScale = 2.f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.f;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->MaxFlySpeed = 600.f;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	// This is the sphere of aggrevation! When a player enters it, we do a line of sight test and then make our bad guy go ape on them!
	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Agro Sphere"));
	AgroSphere->SetupAttachment(RootComponent);
	AgroSphere->InitSphereRadius(105.0f);
	AgroSphere->bGenerateOverlapEvents = true;
	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseEnemy::OnAgroOverlap);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &ABaseEnemy::OnAgroEndOverlap);
	AgroSphere->SetCollisionProfileName(AGRO_CPROFILE);

	// Event Delegates
	OnAttackEvent.AddDynamic(this, &ABaseEnemy::OnAttackBPEvent);
	OnAttackFinishEvent.AddDynamic(this, &ABaseEnemy::OnAttackFinishBPEvent);

	bIsPlayer = false;
}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	// Because the collision components only trigger overlap events after they begin, if the enemy spawns while the player is inside of an overlap sphere, it is not triggered.
	// We call a check here to make sure that it is properly called
	AgroSphere->SetSphereRadius(AgroSphere->GetScaledSphereRadius(), true); // TODO: This is a hack, really we should just get all of the overlapping actors and call the overlap function manually
}

// Called every frame
void ABaseEnemy::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void ABaseEnemy::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if(bRagdoll)
		return;

	// Did we collide with a player character? If so, apply damage to them
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if(Player)
	{
		// For the time being, we'll grab the player's health so that enemy collision does 100% damage
		float Damage = Player->GetHealth();
		Player->TakeDamage(Damage, FPointDamageEvent(), GetController(), this);
	}
}

void ABaseEnemy::OnAgroOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* SensedPawn = Cast<APlayerCharacter>(OtherActor); // Right now they should only detect one actor, the player
	if(!SensedPawn)
		return; // Theoretically this should never happen with the response channels, but it's always good to check

	if( IsPendingKill() )
		return;

	ABaseAIController* AIController = Cast<ABaseAIController>(GetController());
	if(AIController && SensedPawn)
	{
		AIController->SetTargetEnemy(SensedPawn);
	}
}

void ABaseEnemy::OnAgroEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABaseAIController* AIController = Cast<ABaseAIController>(GetController());
	if(AIController)
	{
		//AIController->SetTargetEnemy(nullptr);
		//bSensedTarget = false;
	}
}

bool ABaseEnemy::CanAttack()
{
	if(bIsAttacking)
		return false;

	UCharacterMovementComponent* MoveComp =  GetCharacterMovement();
	if(MoveComp)
	{
		if(MoveComp->IsFalling()) // && !MoveComp->IsFlying())
		{
			return false;
		}
	}

	if(!BPCanAttack)
		return false;

	return true;
}

void ABaseEnemy::Attack()
{
	float Time = AttackTime;

	ABaseWeapon* MyWeapon = GetEquippedWeapon();
	if(MyWeapon)
	{
		Time = MyWeapon->GetFireRate();
		Super::FireWeapon();
	}

	bIsAttacking = true;
	GetWorldTimerManager().SetTimer(AttackTimer, this, &ABaseEnemy::OnAttackFinish, Time, false);

	OnAttackEvent.Broadcast();
}

void ABaseEnemy::OnAttackFinish()
{
	AttackSuccess = true;
	bIsAttacking = false;

	OnAttackFinishEvent.Broadcast();

	ABaseWeapon* MyWeapon = GetEquippedWeapon();
	if(MyWeapon)
	{
		MyWeapon->StopFiring();
	}

	// Let our controller know
	ABaseAIController* AIController = Cast<ABaseAIController>(GetController());
	if(!Controller)
		return;
	
	AIController->OnAttackFinish(AttackSuccess);
}

float ABaseEnemy::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	GetCapsuleComponent()->OnComponentBeginOverlap.RemoveDynamic(this, &ABaseEnemy::OnOverlap);

	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

FVector ABaseEnemy::GetFleeLocation()
{
	// Get our looking direction, reverse that and add our flee distance to get the flee location
	FRotator Direction = GetActorForwardVector().Rotation();

	// Constrain Pitch to 180 and reverse
	Direction.Yaw = FMath::Sign(Direction.Yaw)*-180.0f;
	
	// Remove any roll and Pitch Values
	Direction.Pitch = 0.0f;
	Direction.Roll = 0.0f;

	// Find our fleeing location
	FVector FleeLocation = GetActorLocation() + (Direction.Vector()*FleeDistance);

	return FleeLocation;
}