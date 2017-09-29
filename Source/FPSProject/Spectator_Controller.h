// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Runtime/Engine/Classes/GameFramework/SpringArmComponent.h"
#include "FPSCharacter.h"
#include "FPSPlayerState.h"
#include "FPSPlayerController.h"
#include "Spectator_Controller.generated.h"


UCLASS()
class FPSPROJECT_API ASpectator_Controller : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASpectator_Controller();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USpringArmComponent* CameraBoom;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* MyRoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float CameraBoomLength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxYaw;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MinYaw;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxPitch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MinPitch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
		bool AttachedToMesh = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Replicated)
		class AFPSCharacter* FollowedCharacter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
		class AFPSPlayerController* FollowedController;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
		TArray<class AFPSCharacter*> TeammateCharacters;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
		TArray<class AFPSPlayerState*> TeammateStates;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
		int32 CurrentTeamIndex = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
		FVector LastKnownPlayerLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName PlayerName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float RotationSpeedMultiplier;
	UFUNCTION(Server,WithValidation, Reliable)
		void SetFollowedCharacter(AFPSCharacter* newCharacter);
	UFUNCTION(NetMulticast,Reliable)
		void SetTeammateStates(const TArray<AFPSPlayerState*>& TeamStates);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
		void AddYawRotation(float Value);
	UFUNCTION()
		void AddPitchRotation(float Value);
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerUpdatePosition(FVector NewPosition);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void ServerUpdateYaw(float Value);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void ServerUpdatePitch(float Value);
	UFUNCTION(Server, Reliable,WithValidation)
		void ServerChangePlayer();
	UFUNCTION()
		void UpdatePosition(FVector NewPosition);


	
	
};
