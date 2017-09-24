// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Runtime/Engine/Classes/GameFramework/SpringArmComponent.h"
#include "FPSCharacter.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AFPSCharacter* FollowedCharacter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName PlayerName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float RotationSpeedMultiplier;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UFUNCTION()
		void AddYawRotation(float Value);
	UFUNCTION()
		void AddPitchRotation(float Value);


	
	
};
