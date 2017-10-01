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


void ASpectator_Controller::SetTeammateStates_Implementation(const TArray<AFPSPlayerState*>& TeamStates)
{
	TeammateStates = TeamStates;
	//UE_LOG(LogClass, Log, TEXT("How Many Teammates: %d"), TeammateStates.Num());
}

// Called when the game starts or when spawned
void ASpectator_Controller::BeginPlay()
{
	Super::BeginPlay();
	NetUpdateFrequency = 20.0f;
}

void ASpectator_Controller::ServerUpdatePosition_Implementation(FVector NewPosition)
{
	UpdatePosition(NewPosition);
}

void ASpectator_Controller::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASpectator_Controller, FollowedController);
	DOREPLIFETIME(ASpectator_Controller, FollowedCharacter);
	DOREPLIFETIME(ASpectator_Controller, LastKnownPlayerLocation);
	DOREPLIFETIME(ASpectator_Controller, AttachedToMesh);
	DOREPLIFETIME(ASpectator_Controller, TeammateCharacters);
	DOREPLIFETIME(ASpectator_Controller, TeammateStates);
	DOREPLIFETIME(ASpectator_Controller, CurrentTeamIndex);
	DOREPLIFETIME(ASpectator_Controller, PlayerName);
	DOREPLIFETIME(ASpectator_Controller, MaxPitch);
	DOREPLIFETIME(ASpectator_Controller, MinPitch);
	
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
			SetActorLocation(LastKnownPlayerLocation + FVector(0,0,90.0f));
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
			/*
			if (AttachedToMesh == false)
			{
				FAttachmentTransformRules Rules = FAttachmentTransformRules{ EAttachmentRule::SnapToTarget,false };
				Rules.RotationRule = EAttachmentRule::KeepRelative;
				/*
				if (Rules != NULL)
				{
					Rules->RotationRule = EAttachmentRule::KeepRelative;
					UE_LOG(LogClass, Log, TEXT("rules exists"));

				}
				else
				{
					UE_LOG(LogClass, Log, TEXT("rules does not exists"));
				}
				
				//FAttachmentTransformRules::RotationRule = EAttachmentRule::KeepRelative;
				MyRoot->AttachToComponent(FollowedCharacter->GetMesh(),Rules);
				//CameraBoom->parent
				AttachedToMesh = true;
			}
			*/
			//MyRoot->SetupAttachment(FollowedCharacter->GetMesh());
			
			LastKnownPlayerLocation = FollowedCharacter->GetMesh()->GetBoneLocation(FName(TEXT("spine_03")));
			//MyRoot->SetWorldRotation(FRotator(0, 0, 0));
			//UE_LOG(LogClass, Log, TEXT("FollowedCharacterName: %s"),*FollowedCharacter->GetActorLocation().ToString());
			UpdatePosition(LastKnownPlayerLocation);
			
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
	PlayerInputComponent->BindAction("ChangeSpectator", IE_Released, this, &ASpectator_Controller::ServerChangePlayer);
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
		//CameraBoom->AddWorldRotation(FRotator(0.0f, RotationValue, 0.0f).Quaternion());
	}
}
void ASpectator_Controller::ServerUpdatePitch_Implementation(float Value)
{
	if (FollowedCharacter)
	{
		float RotationValue = -Value * RotationSpeedMultiplier;
		if (GetActorRotation().Pitch + RotationValue < MaxPitch && GetActorRotation().Pitch + RotationValue > MinPitch)
		{
			
			
			SetActorRotation(FRotator(GetActorRotation().Pitch + RotationValue, GetActorRotation().Yaw, GetActorRotation().Roll));
		}
		else
		{
			//SetActorRotation(FRotator(GetActorRotation().Pitch + RotationValue, GetActorRotation().Yaw, GetActorRotation().Roll));
		}
		//AddPitchRotation(RotationValue);
		
		//CameraBoom->AddWorldRotation(FRotator(RotationValue, 0.0f, 0.0f).Quaternion());
	}
}

bool ASpectator_Controller::SetFollowedCharacter_Validate(AFPSCharacter* newCharacter)
{
	return true;
}

void ASpectator_Controller::SetFollowedCharacter_Implementation(AFPSCharacter* newCharacter)
{
	FollowedCharacter = newCharacter;
}

bool ASpectator_Controller::ServerChangePlayer_Validate()
{
	return true;
}

void ASpectator_Controller::ServerChangePlayer_Implementation()
{
	UE_LOG(LogClass, Log, TEXT("I was called"));
	
	if (FollowedCharacter) {
		UE_LOG(LogClass, Log, TEXT("followedcharacter exists"));
		if (FollowedController)
		{
			UE_LOG(LogClass, Log, TEXT("followedcontroller exists"));
			if (TeammateStates.Num() > 0) {
				UE_LOG(LogClass, Log, TEXT("teamstates length > 0"));
				if (AFPSPlayerState* followedCharacterPS = Cast<AFPSPlayerState>(FollowedController->PlayerState))
				{
					if (TeammateStates.Find(followedCharacterPS) != -1)
					{
						UE_LOG(LogClass, Log, TEXT("character is on team"));
						if (CurrentTeamIndex == -1)
						{
							CurrentTeamIndex = TeammateStates.Find(followedCharacterPS);
						}
						if (CurrentTeamIndex == TeammateStates.Num() - 1)
						{
							UE_LOG(LogClass, Log, TEXT("last player, current team index: %d"),CurrentTeamIndex);
							
							for (int32 i = 0; i < TeammateStates.Num(); ++i)
							{
								if (i != CurrentTeamIndex)
								{
									if (TeammateStates[i]->MyCharacter != NULL)
									{
										if (TeammateStates[i]->MyCharacter->GetCurrentState() != EPlayerState::EPlayerDead)
										{
											//FollowedCharacter = TeammateStates[i]->MyCharacter;
											//SetFollowedCharacter(TeammateStates[i]->MyCharacter);
											//UE_LOG(LogClass, Log, TEXT("found player 1"));
											UE_LOG(LogClass, Log, TEXT("new follow name: %s  Index is %d"), *TeammateStates[i]->MyCharacter->GetName(),i);
											FollowedCharacter = TeammateStates[i]->MyCharacter;
											CurrentTeamIndex = i;
											if (AFPSPlayerState* ps = Cast<AFPSPlayerState>(FollowedCharacter->PlayerState))
											{
												PlayerName = FName(*ps->UserName);
											}
											return;
										}
									}
								}
							}
							
							
						}
						else
						{
							UE_LOG(LogClass, Log, TEXT("not last player, current team index: %d"), CurrentTeamIndex);
							
							for (int32 i = CurrentTeamIndex + 1; i < TeammateStates.Num(); ++i)
							{
								if (AFPSPlayerState* ps = Cast<AFPSPlayerState>(TeammateStates[i]))
								{
									if (ps->MyCharacter != NULL)
									{
										if (i != CurrentTeamIndex)
										{
											if (ps->MyCharacter->GetCurrentState() != EPlayerState::EPlayerDead)
											{
												UE_LOG(LogClass, Log, TEXT("new follow2 name: %s Index is: %d"), *TeammateStates[i]->MyCharacter->GetName(),i);
												FollowedCharacter = TeammateStates[i]->MyCharacter;
												CurrentTeamIndex = i;
												if (AFPSPlayerState* ps = Cast<AFPSPlayerState>(FollowedCharacter->PlayerState))
												{
													PlayerName = FName(*ps->UserName);
												}
												return;
											}
										}

										//FollowedCharacter = TeammateStates[i]->MyCharacter;

										//SetFollowedCharacter(TeammateStates[i]->MyCharacter);

									}
									else
									{
										UE_LOG(LogClass, Log, TEXT("MyCharacter is NULL, %d"),i);
									}
								}
								else
								{
									UE_LOG(LogClass, Log, TEXT("cast to player state failed"));
								}

							}
							for (int32 i = 0; i < CurrentTeamIndex; ++i)
							{
								if (AFPSPlayerState* ps = Cast<AFPSPlayerState>(TeammateStates[i]))
								{
									if (ps->MyCharacter != NULL)
									{
										if (ps->MyCharacter->GetCurrentState() != EPlayerState::EPlayerDead)
										{
											UE_LOG(LogClass, Log, TEXT("new follow3 name: %s  Index is: %d"), *TeammateStates[i]->MyCharacter->GetName(),i);
											FollowedCharacter = TeammateStates[i]->MyCharacter;
											CurrentTeamIndex = i;
											if (AFPSPlayerState* ps = Cast<AFPSPlayerState>(FollowedCharacter->PlayerState))
											{
												PlayerName = FName(*ps->UserName);
											}
											return;
										}

										//FollowedCharacter = TeammateStates[i]->MyCharacter;

										//SetFollowedCharacter(TeammateStates[i]->MyCharacter);

									}
									else
									{
										UE_LOG(LogClass, Log, TEXT("MyCharacter is NULL, %d"), i);
									}
								}
								else
								{
									UE_LOG(LogClass, Log, TEXT("cast to player state failed"));
								}
							}
							

						}
					}

				}
			}
		}
	}
	
	
}
