// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "BaseGrenade.h"
#include "FragGrenade.h"
#include "FPSPlayerState.h"
#include "BaseEquipment.h"
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

	UPROPERTY(Replicated,EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		TArray<class AGun*> MyWeapons;
	UPROPERTY(Replicated,EditAnywhere, BlueprintReadOnly)
		FString PickupWeaponText;
	UPROPERTY(EditAnywhere, Category = "Weapon")
		float WeaponSwitchDelay = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Weapon")
		FRotator RecoilGoalRotation;
	UPROPERTY(EditAnywhere, Category = "Weapon")
		FTimerHandle WeaponSwitchDelayTimer;
	UPROPERTY(Replicated, EditAnywhere, Category = "Weapon")
		bool bCanSwitchWeapon = true;
	UFUNCTION()
		void SetCanSwitchWeapon();
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
		TSubclassOf<ABaseGrenade> CurrentGrenade;
	UPROPERTY(EditAnywhere)
		class AGun* PrimaryInstance;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
		class AGun* SecondaryInstance;
		//TSubobjectPtr<class AGun> SecondaryInstance;

	UFUNCTION(Server, Reliable, WithValidation)
		void ThrowGrenade();

	FTimerHandle DeadWeaponDropTimer;
	UPROPERTY(EditAnywhere)
		float DeadWeaponDropDelay = 1.5f;
	UFUNCTION()
		void DeadDropWeapon();
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
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
		bool IsRecoiling = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float HealthPercentage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DefaultFOV;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Weapon")
		int32 MaxNumberOfWeapons = 2;


	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "CrossHair")
		FVector Right_CHLocation;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "CrossHair")
		FVector Left_CHLocation;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "CrossHair")
		FVector Top_CHLocation;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "CrossHair")
		FVector Bottom_CHLocation;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "CrossHair")
		FVector Right_CHDirection;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "CrossHair")
		FVector Left_CHDirection;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "CrossHair")
		FVector Top_CHDirection;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "CrossHair")
		FVector Bottom_CHDirection;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "CrossHair")
		FVector ShotStartLocation;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "CrossHair")
		FVector ShotDirection;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "CrossHair")
		float ShotLocation_MaxX;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "CrossHair")
		float ShotLocation_MaxY;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "CrossHair")
		float ShotLocation_MinX;
	UPROPERTY(Replicated,EditAnywhere, BlueprintReadWrite, Category = "CrossHair")
		float ShotLocation_MinY;



	//handling player state
	UFUNCTION(BlueprintPure)
		EPlayerState GetCurrentState() const;
	void SetCurrentState(EPlayerState NewState);
	UFUNCTION(BlueprintCallable, Category = "Health")
		float GetCurrentHealth();
	UFUNCTION(BlueprintImplementableEvent)
		void GenerateShot();
	UFUNCTION(BlueprintCallable)
		void SetShot(FVector Location,FVector Direction);
	UFUNCTION(Server,WithValidation,Reliable)
		void SetShotMULTI(FVector Location, FVector Direction);
	UFUNCTION(Server,WithValidation,Reliable)
		void GenerateShotMULTI();
	UFUNCTION(Reliable, NetMultiCast, WithValidation, Category = "Weapon")
		void Zoom();
	UFUNCTION(Reliable, NetMultiCast, WithValidation, Category = "Weapon")
		void SetIsZoomed(bool zoomvalue);
	UFUNCTION(Reliable, NetMultiCast, Category = "Weapon")
		void SetIsRecoiling(bool recoilvalue);
	UFUNCTION(Reliable, NetMultiCast)
		void SetPickingUpWeapon(bool IsPickingUpWeapon);
	UFUNCTION(BlueprintCallable, Category = "Health")
		float GetInitialHealth();
	UFUNCTION(NetMultiCast, Reliable, Category = "Health")
		void ServerChangeHealthBy(float delta);
	UFUNCTION(BlueprintCallable,Category = "Health")
		void BeginRechargeHealth();
	UFUNCTION(BlueprintCallable, Category = "Health")
		void RechargeHealth();
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

	UFUNCTION()//NetMulticast, Reliable)
		void DropWeapon();
	UFUNCTION(NetMulticast,Reliable)
		void DropEquipment();
	UPROPERTY(Replicated, EditAnywhere)
		bool PickingUpWeapon = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//Entry. Called when player presses a key to collect pickups
	UFUNCTION(Reliable,Server, WithValidation,Category = "Weapon")
		void PickupWeapon();
	UFUNCTION(Reliable, Server, WithValidation, Category = "Weapon")
		void SwitchWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void PickupEquipment();

	UFUNCTION(Server,Reliable,WithValidation, Category = "Equipment")
		void ServerDropEquipment();


	UFUNCTION(Client, Reliable, WithValidation, Category = "Equipment")
		void ClientDropEquipment();
	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void FireAgain();
	UPROPERTY(EditDefaultsOnly, Category = "Health")
		float InitialHealth;
	UPROPERTY(EditDefaultsOnly, Category = "Health")
		float CurrentHealth;
	UPROPERTY(EditDefaultsOnly, Category = "Health")
		float HealthRechargeDelay = 5.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Health")
		float HealthRechargeSpeed = 3.0f;
	//called on server to process the collection of pickups
	UFUNCTION(Reliable, NetMulticast, WithValidation)
		void ServerPickupWeapon();
	UFUNCTION(Reliable, NetMulticast)
		void ServerSwitchWeapon();
	UFUNCTION(Reliable, Server, WithValidation)
		void ServerPickupEquipment();

	UFUNCTION(Reliable, Server, WithValidation)
		void ServerDropWeapon();
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		FTimerHandle WeaponFireRateTimer;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		FTimerHandle ZoomTimer;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		FTimerHandle RecoilTimer;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		FTimerHandle HealthRechargeTimer;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		FTimerHandle HealthRechargeDELTATimer;
	UPROPERTY(Replicated,EditAnywhere)
		FVector MyDeathLocation;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		bool IsFiring;

	UPROPERTY(VisibleAnywhere)
		TEnumAsByte<enum EPlayerState> CurrentState;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(Replicated,EditAnywhere, Category = "death")
		bool IsDead = false;

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

	UFUNCTION(BlueprintImplementableEvent)
		void OnDeathRemoveUI();
	//TSubclassOf<AGun> CurrentPrimary;

	UFUNCTION(Server,WithValidation, Reliable)
		void ServerReload();
	// First-person mesh (arms), visible only to the owning player.
	UPROPERTY(Replicated, VisibleDefaultsOnly,BlueprintReadWrite, Category = Mesh)
		USkeletalMeshComponent* FPSMesh;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void OnShoot();

	UFUNCTION(Reliable, Server, WithValidation)
		virtual void ServerOnZoom();
	UFUNCTION(Reliable, NetMulticast)
		virtual void AddRecoil();
	UFUNCTION(Reliable, NetMulticast, WithValidation)
		void GrenadeNearby();
	UFUNCTION(Reliable, Server, WithValidation)
		virtual void ServerOnStopZoom();

	UFUNCTION(Reliable, Server, WithValidation)
		virtual void ServerOnShoot();


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
	FTimerHandle DeathTimer;
	UFUNCTION(Server,Reliable,WithValidation)
	void DestroySelf();

};
