// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseGrenade.h"
#include "GrenadeSpawner.generated.h"

UCLASS()
class FPSPROJECT_API AGrenadeSpawner : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AGrenadeSpawner();

	UPROPERTY(EditAnywhere, Replicated, Category = "Weapon")
		TSubclassOf<class ABaseGrenade> ReferencedGrenade;

	UPROPERTY(EditAnywhere, Replicated, Category = "WeaponSpawning")
		float RespawnTimerDelay = 120.0f;
	UPROPERTY(EditAnywhere, Replicated, Category = "WeaponSpawning")
		bool SpawnOnStart = true;

	UPROPERTY(EditAnywhere, Replicated, Category = "Transform")
		FRotator GrenadeRotation = FRotator::ZeroRotator;

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
	void SpawnGrenade();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
	
};
