// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Base/LnBCharacter.h"
#include "PlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class LNB_API APlayerCharacter : public ALnBCharacter
{
	GENERATED_BODY()

	/** Side view camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* SideViewCameraComponent;

	/** Camera boom positioning the camera beside the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	// An input vector for an analog stick, controller, etc, that we transfer into a look direction
	FVector NewAimInput;

	// Overrided to handle two weapon slots
	virtual void EquipWeapon(ABaseWeapon* NewWeapon) override;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LNB|PlayerCharacter")
	ABaseWeapon* SavedWeapon = NULL;

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* Input) override;
	// End of APawn interface

	/** Called for side to side input */
	void MoveRight(float Val);

	/** Called for looking up, down, left and right*/
	void LookUp(float Val);
	void LookRight(float Val);

	/** Handle touch inputs. */
	void TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location);

	/** Handle touch stop event. */
	void TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location);

	/** Handles shoot inputs. */
	void StartFiring();

	/** Handles shoot inputs. */
	void StopFiring();

	/** Handles changing weapon inputs. */
	void ChangeWeapon();

	void StartCrouch();
	void EndCrouch();
		
public:
	APlayerCharacter();
	virtual void Tick(float Delta) override;

	/** Returns SideViewCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	// Callback function for ItemCollisionComp
	UFUNCTION()
	void OnItemBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	virtual void FireWeapon() override;

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// Handle the game over state here
	virtual void LifeSpanExpired() override;
};
