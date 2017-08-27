// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "BaseGrenade.h"
#include "FragGrenade.h"

#include "Runtime/Engine/Public/CollisionQueryParams.h"
#include "FPSGameState.h"
#include "Gun.h"
#include "FPSCharacter.generated.h"


UENUM(BlueprintType)
enum EPlayerState
{
	EPlayerWaiting,
	EPlayerPlaying,
	EPlayerDead,
	EPlayerUnknown
};

UCLASS()
class FPSPROJECT_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

		/** Collection Sphere */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* CollectionBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* GrenadeDetectionBox;

public:
	// Sets default values for this character's properties
	AFPSCharacter();
	UFUNCTION()
		void Update();
	UPROPERTY(EditAnywhere, Category = "Update")
		float UpdateDelay;
	UPROPERTY(Replicated,EditAnywhere, Category = "Grenade")
		float GrenadeThrowStrength = 30.0f;
	UPROPERTY(Replicated,EditAnywhere, Category = "Grenade")
		float GrenadeThrowUpForce = 30.0f;
	UPROPERTY(BlueprintReadWrite,EditAnywhere, Category = "Grenade")
		float GrenadeDegrees = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grenade")
		FVector2D GrenadeDir;
	UPROPERTY(Replicated,BlueprintReadWrite, EditAnywhere, Category = "Grenade")
		bool bIsNearGrenade = false;

	UFUNCTION(NetMulticast, Reliable)
		void ShowEnemyName(class AFPSCharacter* Enemy);
	UFUNCTION(BlueprintImplementableEvent)
		void ShowEnemyNameBluePrint(class AFPSCharacter* Enemy);
	UPROPERTY(Replicated,EditAnywhere,BlueprintReadWrite)
	TArray<ABaseGrenade*> Grenades;
	UPROPERTY(EditAnywhere)	
		TSubclassOf<ABaseGrenade> FragSUB;

	UFUNCTION(Server, Reliable, WithValidation)
		void ThrowGrenade();

	FTimerHandle UpdateTimer;
	UFUNCTION(NetMultiCast, Reliable, WithValidation)
		void TriggerDeathUI();
	UFUNCTION(BlueprintNativeEvent)
		void TriggerDeathUIBlueprint();
	UFUNCTION()
		void TriggerUpdateUI();
	UFUNCTION(BlueprintNativeEvent)
		void TriggerUpdateUIBlueprint();
	UFUNCTION(Server, Reliable, WithValidation)
		void TriggerAddUI();
	UFUNCTION(BlueprintNativeEvent)
		void TriggerAddUIBlueprint();
	UFUNCTION(NetMultiCast, Reliable)
		void AddTeamColor();
	UFUNCTION(BlueprintNativeEvent)
		void TriggerAddColorBlueprint();

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
		float AccuracySpreadValue = 0;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
		float simASV = 0;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
		bool IsZoomed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float HealthPercentage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DefaultFOV;

	//handling player state
	UFUNCTION(BlueprintPure)
		EPlayerState GetCurrentState() const;
	void SetCurrentState(EPlayerState NewState);
	UFUNCTION(BlueprintCallable, Category = "Health")
		float GetCurrentHealth();
	UFUNCTION(Reliable, NetMultiCast, WithValidation, Category = "Health")
		void Zoom();
	UFUNCTION(Reliable, NetMultiCast, WithValidation, Category = "Health")
		void SetIsZoomed(bool zoomvalue);
	UFUNCTION(BlueprintCallable, Category = "Health")
		float GetInitialHealth();
	UFUNCTION(NetMultiCast, Reliable, Category = "Health")
		void ServerChangeHealthBy(float delta);
	//required network scaffolding
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UPROPERTY(Replicated, EditAnywhere, Category = "Damage")
		float LastHitForce;
	UPROPERTY(Replicated, EditAnywhere, Category = "Damage")
		FName LastHitBone;
	UPROPERTY(Replicated, EditAnywhere, Category = "Damage")
		FVector LastHitDirection;
	// shut down the pawn and ragdoll it on all clients
	UFUNCTION(NetMulticast,Reliable) 
		void OnPlayerDeath();
	//UFUNCTION(Server, WithValidation, Reliable) //multicast function happens on server and clients so everyone will see the player ragdoll
	//	void ServerOnPlayerDeath();
	UFUNCTION(NetMultiCast, Reliable) //multicast function happens on server and clients so everyone will see the player ragdoll
		void SetHitData(float Force,FName Bone,FVector Direction);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//Entry. Called when player presses a key to collect pickups
	UFUNCTION(Reliable,Server, WithValidation,Category = "Weapon")
		void PickupWeapon();
	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void PickupEquipment();
	UFUNCTION(Category = "Weapon")
		void DropWeapon();
	UFUNCTION(Server,Reliable,WithValidation, Category = "Equipment")
		void ServerDropEquipment();
	UFUNCTION()
		void DropEquipment();
	UFUNCTION(Client, Reliable, WithValidation, Category = "Equipment")
		void ClientDropEquipment();
	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void FireAgain();
	UPROPERTY(EditDefaultsOnly, Category = "Health")
		float InitialHealth;
	UPROPERTY(EditDefaultsOnly, Category = "Health")
		float CurrentHealth;
	//called on server to process the collection of pickups
	UFUNCTION(Reliable, NetMulticast, WithValidation)
		void ServerPickupWeapon();
	UFUNCTION(Reliable, Server, WithValidation)
		void ServerPickupEquipment();

	UFUNCTION(Reliable, Server, WithValidation)
		void ServerDropWeapon();
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		FTimerHandle WeaponFireRateTimer;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		FTimerHandle ZoomTimer;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		bool IsFiring;
	UPROPERTY(EditAnywhere, Category = "death")
		bool IsDead = false;
	UPROPERTY(VisibleAnywhere)
		TEnumAsByte<enum EPlayerState> CurrentState;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Handles input for moving forward and backward.
	UFUNCTION()
		void MoveForward(float Value);

	// Handles input for moving right and left.
	UFUNCTION()
		void MoveRight(float Value);

	// Sets jump flag when key is pressed.
	UFUNCTION()
		void StartJump();

	// Clears jump flag when key is released.
	UFUNCTION()
		void StopJump();

	// FPS camera.
	UPROPERTY(Replicated, VisibleAnywhere)
		UCameraComponent* FPSCameraComponent;

	UPROPERTY(Replicated, EditDefaultsOnly,BlueprintReadOnly, Category = "Weapon")
		class AGun* CurrentPrimary;
	//TSubclassOf<AGun> CurrentPrimary;

	UFUNCTION(Server,WithValidation, Reliable)
		void ServerReload();
	// First-person mesh (arms), visible only to the owning player.
	UPROPERTY(Replicated, VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* FPSMesh;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void OnShoot();

	UFUNCTION(Reliable, Server, WithValidation)
		virtual void ServerOnZoom();
	UFUNCTION(Reliable, NetMulticast, WithValidation)
		void GrenadeNearby();
	UFUNCTION(Reliable, Server, WithValidation)
		virtual void ServerOnStopZoom();

	UFUNCTION(Reliable, Server, WithValidation)
		virtual void ServerOnShoot();
	UFUNCTION(Reliable, NetMultiCast, WithValidation)
		virtual void ServerAddGunRecoil(float RecoilValue);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void OnStopShoot();


	UFUNCTION(Reliable, Server, WithValidation)
		virtual void ServerOnStopShoot();

	UPROPERTY(EditAnywhere, Category = "Weapon")
		bool HasWeapon;


	UFUNCTION(Server, Reliable, WithValidation)
		void ServerUpdateRotation(FRotator Rotation);

	UPROPERTY(Replicated, EditDefaultsOnly)
		APawn* Shooter;

};
