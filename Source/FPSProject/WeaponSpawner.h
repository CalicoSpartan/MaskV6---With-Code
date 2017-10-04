// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun.h"
#include "WeaponSpawner.generated.h"

UCLASS()
class FPSPROJECT_API AWeaponSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponSpawner();

	UPROPERTY(EditAnywhere, Replicated, Category = "Weapon")
		TSubclassOf<class AGun> ReferencedWeapon;
	UPROPERTY(EditAnywhere, Replicated, Category = "WeaponStats")
		int32 TotalAmmo;
	UPROPERTY(EditAnywhere, Replicated, Category = "WeaponStats")
		int32 MagazineAmmo;
	UPROPERTY(EditAnywhere, Replicated, Category = "WeaponSpawning")
		float RespawnTimerDelay = 120.0f;
	UPROPERTY(EditAnywhere, Replicated, Category = "WeaponSpawning")
		bool SpawnOnStart = true;

	UPROPERTY(EditAnywhere, Replicated, Category = "Transform")
		FRotator WeaponRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, Replicated, Category = "WeaponSpawning")
		FTimerHandle RespawnTimer;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetRespawnTimer();
	UFUNCTION()
		void ClientSetRespawnTimer();

	void SetRespawnTimer();
	void SpawnWeapon();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
