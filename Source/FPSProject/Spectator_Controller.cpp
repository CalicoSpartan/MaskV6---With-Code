// Fill out your copyright notice in the Description page of Project Settings.

#include "Spectator_Controller.h"


// Sets default values
ASpectator_Controller::ASpectator_Controller()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASpectator_Controller::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpectator_Controller::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASpectator_Controller::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ASpectator_Controller::AddYawRotation(float Value)
{
}

void ASpectator_Controller::AddPitchRotation(float Value)
{
}

