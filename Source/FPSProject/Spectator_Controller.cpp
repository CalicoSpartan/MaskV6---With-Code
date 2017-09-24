// Fill out your copyright notice in the Description page of Project Settings.

#include "Spectator_Controller.h"


// Sets default values
ASpectator_Controller::ASpectator_Controller()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("MyRoot"));
	RootComponent = MyRoot;
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
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
	if (GetController())
	{
		//UE_LOG(LogClass, Log, TEXT("Has Controller"));
	}
	else
	{
		//UE_LOG(LogClass, Log, TEXT("NO CONTROLLER"));
	}
	if (FollowedCharacter)
	{
		//SetActorLocation(FollowedCharacter->GetActorLocation());
		//CameraBoom->SetWorldLocation(FollowedCharacter->GetActorLocation());
	}
}

// Called to bind functionality to input
void ASpectator_Controller::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("Turn", this, &ASpectator_Controller::AddYawRotation);
	PlayerInputComponent->BindAxis("LookUp", this, &ASpectator_Controller::AddPitchRotation);
}

void ASpectator_Controller::AddYawRotation(float Value)
{
	if (FollowedCharacter)
	{
		float RotationValue = Value * RotationSpeedMultiplier;
		SetActorRotation(FRotator(GetActorRotation().Pitch, GetActorRotation().Yaw + RotationValue, GetActorRotation().Roll));
		//AddYawRotation(RotationValue);
		//CameraBoom->AddLocalRotation(FRotator(0.0f, RotationValue, 0.0f).Quaternion());
	}
}

void ASpectator_Controller::AddPitchRotation(float Value)
{
	if (FollowedCharacter)
	{
		
		float RotationValue = Value * RotationSpeedMultiplier;
		SetActorRotation(FRotator(GetActorRotation().Pitch, GetActorRotation().Yaw + RotationValue, GetActorRotation().Roll));
		//AddPitchRotation(RotationValue);
		//CameraBoom->AddLocalRotation(FRotator(RotationValue, 0.0f, 0.0f).Quaternion());
	}
}

