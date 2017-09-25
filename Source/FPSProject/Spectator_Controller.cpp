// Fill out your copyright notice in the Description page of Project Settings.

#include "Spectator_Controller.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ASpectator_Controller::ASpectator_Controller()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bReplicateMovement = true;
	MyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("MyRoot"));
	RootComponent = MyRoot;
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	//CameraBoom->bUsePawnControlRotation = true;
	//bUseControllerRotationPitch = true;
	//bUseControllerRotationYaw = true;
}

// Called when the game starts or when spawned
void ASpectator_Controller::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASpectator_Controller::ServerUpdatePosition_Implementation(FVector NewPosition)
{
	UpdatePosition(NewPosition);
}

void ASpectator_Controller::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASpectator_Controller, FollowedCharacter);
	DOREPLIFETIME(ASpectator_Controller, LastKnownPlayerLocation);
	DOREPLIFETIME(ASpectator_Controller, AttachedToMesh);
}

bool ASpectator_Controller::ServerUpdatePosition_Validate(FVector NewPosition)
{
	return true;
}
void ASpectator_Controller::UpdatePosition(FVector NewPosition)
{
	
	if (FollowedCharacter != NULL)
	{
		
		//UE_LOG(LogClass, Log, TEXT("%s"), *LastKnownPlayerLocation.ToString());
		if (Role == ROLE_Authority)
		{
			SetActorLocation(LastKnownPlayerLocation);
			//UE_LOG(LogClass, Log, TEXT("location: %s"), *FollowedCharacter->GetMesh()->GetComponentLocation().ToString());
			
		}
		else
		{
			UE_LOG(LogClass, Log, TEXT("Client"));
		}
		
		//SetActorLocation(FollowedCharacter->GetMesh()->GetComponentLocation());
		//CameraBoom->SetWorldLocation(FollowedCharacter->GetMesh()->GetComponentLocation());
		//CameraBoom->SetWorldLocation(FollowedCharacter->GetActorLocation());
	}
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
	
	if (Role == ROLE_Authority)
	{

		if (FollowedCharacter)
		{
			if (AttachedToMesh == false)
			{
				//FAttachmentTransformRules* Rules = nullptr;
				
				//Rules->RotationRule = EAttachmentRule::KeepRelative;
			
				//FAttachmentTransformRules::RotationRule = EAttachmentRule::KeepRelative;
				MyRoot->AttachToComponent(FollowedCharacter->GetMesh(),FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				//CameraBoom->parent
				AttachedToMesh = true;
			}
			//MyRoot->SetupAttachment(FollowedCharacter->GetMesh());
			LastKnownPlayerLocation = FollowedCharacter->GetMesh()->GetComponentLocation();
			//UE_LOG(LogClass, Log, TEXT("Server"));
			//UpdatePosition(LastKnownPlayerLocation);
			
		}
	}
	else if (Role == ROLE_AutonomousProxy)
	{
		if (FollowedCharacter)
		{
			//UE_LOG(LogClass, Log, TEXT("AutonomousProxy"));
			//ServerUpdatePosition(FollowedCharacter->GetMesh()->GetComponentLocation());
		}
	}
	else
	{
		//UE_LOG(LogClass, Log, TEXT("Unknown role"));
		//UpdatePosition();
	}
	
}

// Called to bind functionality to input
void ASpectator_Controller::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("Turn", this, &ASpectator_Controller::ServerUpdateYaw);
	PlayerInputComponent->BindAxis("LookUp", this, &ASpectator_Controller::ServerUpdatePitch);
}

void ASpectator_Controller::AddYawRotation(float Value)
{

}

void ASpectator_Controller::AddPitchRotation(float Value)
{

}

bool ASpectator_Controller::ServerUpdateYaw_Validate(float Value)
{
	return true;
}
bool ASpectator_Controller::ServerUpdatePitch_Validate(float Value)
{
	return true;
}

void ASpectator_Controller::ServerUpdateYaw_Implementation(float Value)
{
	if (FollowedCharacter)
	{
		float RotationValue = Value * RotationSpeedMultiplier;
		SetActorRotation(FRotator(GetActorRotation().Pitch, GetActorRotation().Yaw + RotationValue, GetActorRotation().Roll));
		//AddYawRotation(RotationValue);
		//CameraBoom->AddLocalRotation(FRotator(0.0f, RotationValue, 0.0f).Quaternion());
	}
}
void ASpectator_Controller::ServerUpdatePitch_Implementation(float Value)
{
	if (FollowedCharacter)
	{

		float RotationValue = Value * RotationSpeedMultiplier;
		SetActorRotation(FRotator(GetActorRotation().Pitch + RotationValue, GetActorRotation().Yaw, GetActorRotation().Roll));
		//AddPitchRotation(RotationValue);
		//CameraBoom->AddLocalRotation(FRotator(RotationValue, 0.0f, 0.0f).Quaternion());
	}
}
