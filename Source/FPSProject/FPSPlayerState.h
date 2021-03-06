// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseTeam.h"
#include "GameFramework/PlayerState.h"
#include "FPSCharacter.h"
#include "FPSPlayerState.generated.h"

/**
*
*/
UCLASS()
class FPSPROJECT_API AFPSPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Team")
		bool bTeamB;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Team")
		int32 TeamNumber = 0;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
		int32 Betrayals = 0;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Team")
		FString UserName = "000";
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Team")
		FString PlayerTag = "000";

	//required network scaffolding
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION(BlueprintCallable, Category = "Stats")
		FString GetUserName();
	UFUNCTION(BlueprintCallable, Category = "Stats")
		int32 GetScore();
	UFUNCTION(BlueprintCallable, Category = "Stats")
		int32 GetKills();
	UFUNCTION(BlueprintCallable, Category = "Stats")
		int32 GetDeaths();
	UFUNCTION(BlueprintCallable, Category = "Stats")
		int32 GetAssists();
	UFUNCTION(NetMultiCast, Reliable, Category = "Stats")
		void SetScoreMultiCast(int32 delta);
	UFUNCTION(NetMultiCast, Reliable, Category = "Stats")
		void SetUserNameMultiCast(FName NewName);
	UFUNCTION(NetMultiCast, Reliable, Category = "Stats")
		void SetPlayerTagMultiCast(FName NewTag);
	UFUNCTION(NetMultiCast, Reliable, Category = "Stats")
		void SetKillsMultiCast(int32 delta);
	UFUNCTION(NetMultiCast, Reliable, Category = "Stats")
		void SetDeathsMultiCast(int32 delta);
	UFUNCTION(NetMultiCast, Reliable, Category = "Stats")
		void SetAssistsMultiCast(int32 delta);
	UFUNCTION(NetMultiCast, Reliable, Category = "Team")
		void SetTeam(class ABaseTeam* MyTeam);
	UFUNCTION(NetMultiCast, Reliable, Category = "Team")
		void SetRivalTeam(class ABaseTeam* NewRivalTeam);
	UFUNCTION()
		void SetMyCharacter(class AFPSCharacter* NewCharacter);
	UFUNCTION()
		void BeginPlaySetMyCharacter(class AFPSCharacter* NewCharacter);
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetMyCharacter(class AFPSCharacter* NewCharacter);

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Team")
		class ABaseTeam* RivalTeam;

	UPROPERTY(Replicated,EditAnywhere, BlueprintReadOnly,Category = "Team")
		class ABaseTeam* Team;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Team")
		class AFPSCharacter* MyCharacter;


private:

	UPROPERTY(Replicated, VisibleAnywhere, Category = "Stats")
		int32 MyScore = 0;
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Stats")
		int32 Kills = 0;
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Stats")
		int32 Deaths = 0;
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Stats")
		int32 Assists = 0;
};
