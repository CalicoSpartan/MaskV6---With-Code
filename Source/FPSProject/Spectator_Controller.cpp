// Fill out your copyright notice in the Description page of Project Settings.

#include "Spectator_Controller.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ASpectator_Controller::ASpectator_Controller()
{

	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bReplicateMovement = true;
	MyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("MyRoot"));
	RootComponent = MyRoot;
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

}


void ASpectator_Controller::SetTeammateStates_Implementation(const TArray<AFPSPlayerState*>& TeamStates)
{
	TeammateStates = TeamStates;
}

// Called when the game starts or when spawned
void ASpectator_Controller::BeginPlay()
{
	Super::BeginPlay();
	NetUpdateFrequency = 20.0f;
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


void ASpectator_Controller::UpdatePosition(FVector something)
{
	
	if (FollowedCharacter != NULL)
	{
		
		
		if (Role == ROLE_Authority)
		{
			SetActorLocation(LastKnownPlayerLocation + FVector(0,0,90.0f));
			
			
		}
		

	}
}
// Called every frame
void ASpectator_Controller::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	if (Role == ROLE_Authority)
	{

		if (FollowedCharacter)
		{
			
			LastKnownPlayerLocation = FollowedCharacter->GetMesh()->GetBoneLocation(FName(TEXT("spine_03")));
			UpdatePosition(FVector::ZeroVector);
			
		}
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
			
		}

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
	if (AFPSGameState* gameState = Cast<AFPSGameState>(GetWorld()->GetGameState()))
	{
		if (gameState->GetCurrentState() != EGamePlayState::EGameOver)
		{



			if (FollowedCharacter) {
				if (FollowedController)
				{
					if (TeammateStates.Num() > 0) {
						if (AFPSPlayerState* followedCharacterPS = Cast<AFPSPlayerState>(FollowedController->PlayerState))
						{
							if (TeammateStates.Find(followedCharacterPS) != -1)
							{

								if (CurrentTeamIndex == -1)
								{
									CurrentTeamIndex = TeammateStates.Find(followedCharacterPS);
								}
								if (CurrentTeamIndex == TeammateStates.Num() - 1)
								{


									for (int32 i = 0; i < TeammateStates.Num(); ++i)
									{
										if (i != CurrentTeamIndex)
										{
											if (TeammateStates[i]->MyCharacter != NULL)
											{
												if (TeammateStates[i]->MyCharacter->GetCurrentState() != EPlayerState::EPlayerDead)
												{
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
									for (int32 i = 0; i < CurrentTeamIndex; ++i)
									{
										if (AFPSPlayerState* ps = Cast<AFPSPlayerState>(TeammateStates[i]))
										{
											if (ps->MyCharacter != NULL)
											{
												if (ps->MyCharacter->GetCurrentState() != EPlayerState::EPlayerDead)
												{
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
							}

						}
					}
				}
			}
		}
	}
	
	
}
