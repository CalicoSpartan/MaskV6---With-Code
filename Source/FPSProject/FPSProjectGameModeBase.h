// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FPSGameState.h"
#include "BaseTeam.h"
#include "WeaponSpawner.h"
#include "GrenadeSpawner.h"
#include "FPSPlayerState.h"
#include "FPSPlayerStart.h"
#include "FPSPlayerController.h"
#include "FPSProjectGameModeBase.generated.h"

/**
*
*/
UCLASS()
class FPSPROJECT_API AFPSProjectGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFPSProjectGameModeBase();
	bool callscoreboard = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameStats")
		TArray<class AFPSPlayerController*> Team1PlayerControllers;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameStats")
		TArray<class AFPSPlayerController*> Team2PlayerControllers;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teams")
		TArray<ABaseTeam*> Teams;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponSpawning")
		TArray<class AWeaponSpawner*> WeaponSpawns;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponSpawning")
		TArray<class AGrenadeSpawner*> GrenadeSpawns;

	//NEW STUFF////////////////////////////////////////////////////////////////////////////////
	virtual void StartNewPlayer(APlayerController* NewPlayer);

	//NEW STUFF////////////////////////////////////////////////////////////////////////////////
	virtual void StartNewPlayerClient(APlayerController* NewPlayer);

	virtual void RespawnPlayer(APlayerController* NewPlayer);
	UPROPERTY(BlueprintReadOnly,EditAnywhere)
	FTimerHandle GameCountdownTimer;

	UPROPERTY(BlueprintReadOnly,EditAnywhere, Category = "Rules")
		float GameCountdownDelay = 10.0f;

	UFUNCTION()
		void AllowPlay();


	void PostLogin(APlayerController * NewPlayer) override;

	//virtual AActor * FindPlayerStart_Implementation(AController * Player, const FString & IncomingName) override;

	//AActor * ChoosePlayerStart_Implementation(AController * Player) override;

	bool ShouldSpawnAtStartSpot(AController * Player) override { return false; };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 NumberOfTeams;


	bool HasLoggedIn = false;
	uint8 NumTeamA;
	uint8 NumTeamB;
	int32 PointsPerKill;

	int32 ScoreToWin;
	int32 Team1Players;
	int32 Team2Players;
	int32 Team3Players;
	int32 Team4Players;
	int32 Team5Players;

	UPROPERTY(EditDefaultsOnly)
		int32 SafeSpawnDistance;
	UPROPERTY(EditAnywhere, Category = "Spawning")
		float RespawnTimer = 5.0f;


	virtual void BeginPlay() override;
	UPROPERTY(BlueprintReadWrite)
		FString LastKiller;
	UPROPERTY(BlueprintReadWrite)
		FString LastVictim;
	UPROPERTY(BlueprintReadWrite)
		bool AddKillFeedEntry;
	UFUNCTION(BlueprintImplementableEvent)
		void ActuallyAddKillFeedEntry();
	UFUNCTION()
		void KillPlayer(AFPSCharacter* Killed, AFPSCharacter* Killer);
	UFUNCTION()
		void UpdateStats();
	UPROPERTY(VisibleAnywhere, Category = "Rules")
		int32 SpawnDelay;
	UPROPERTY(VisibleAnywhere, Category = "Spawning")
		TArray<AFPSPlayerStart*> Team1PlayerStarts;
	UPROPERTY(VisibleAnywhere, Category = "Spawning")
		TArray<AFPSPlayerStart*> Team2PlayerStarts;
	UPROPERTY(VisibleAnywhere, Category = "Spawning")
		TArray<AFPSPlayerStart*> Team3PlayerStarts;
	UPROPERTY(VisibleAnywhere, Category = "Spawning")
		TArray<AFPSPlayerStart*> Team4PlayerStarts;
	UPROPERTY(VisibleAnywhere, Category = "Spawning")
		TArray<AFPSPlayerStart*> Team5PlayerStarts;
	UPROPERTY(EditAnywhere, Category = "Teams")
		TSubclassOf<class ABaseTeam> BaseTeamObj;
protected:
	//how many times per second to update the game's UI and State

private:


	//handfle function calls for when the game transitions between states
	void HandleNewState(enum EGamePlayState NewState);

public:
	virtual void Tick(float DeltaTime) override;

};

