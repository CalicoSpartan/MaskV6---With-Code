// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSPlayerController.h"
#include "FPSProjectGameModeBase.h"
#include "Net/UnrealNetwork.h"


void AFPSPlayerController::OnKilled()
{
	followedcharacter = NULL;
	if (AFPSCharacter* mycharacter = Cast<AFPSCharacter>(AcknowledgedPawn))
	{
		followedcharacter = mycharacter;
	}
	UnPossess();
	float RespawnDelay = 2.0f;
	if (Role == ROLE_Authority)
	{
		if (AFPSProjectGameModeBase* gameMode = Cast<AFPSProjectGameModeBase>(GetWorld()->GetAuthGameMode()))
		{
			RespawnDelay = gameMode->RespawnTimer;
		}
	}
	
	GetWorldTimerManager().SetTimer(TimerHandle_Respawn, this, &AFPSPlayerController::Respawn, RespawnDelay);
	
	if (Role == ROLE_Authority)
	{
		ASpectator_Controller* SpectatorController = GetWorld()->SpawnActor<ASpectator_Controller>(SpectatorControllerSUB, AcknowledgedPawn->GetActorLocation(), FRotator(-10.0f,0.0f,0.0f));

		if (SpectatorController != NULL)
		{
			UE_LOG(LogClass, Log, TEXT("spectator controller exists"));
		}
		else
		{
			UE_LOG(LogClass, Log, TEXT("spectator controller NULL"));
		}
		if (APlayerController* testcol = Cast<APlayerController>(this))
		{
			if (followedcharacter != NULL)
			{
				if (AFPSPlayerController* me = Cast<AFPSPlayerController>(this))
				{
					SpectatorController->FollowedController = me;
				}
				
				SpectatorController->FollowedCharacter = followedcharacter;
				if (AFPSPlayerState* characterPlayerState = Cast<AFPSPlayerState>(PlayerState))
				{
					SpectatorController->PlayerName = FName(*characterPlayerState->UserName);
					SpectatorController->SetTeammateStates(characterPlayerState->Team->TeamPlayerStates);
					
				}
				else
				{
					
				}
			}

			PossessCharacter(SpectatorController, testcol);
			UE_LOG(LogClass, Log, TEXT("Possesing"));

		}

		//Possess(SpectatorController);

	}


}

void AFPSPlayerController::PossessCharacter(APawn * character, APlayerController * thecontroller)
{
	thecontroller->Possess(character);
}


void AFPSPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSPlayerController, RivalTeam);
	DOREPLIFETIME(AFPSPlayerController, TeamNumber);
	DOREPLIFETIME(AFPSPlayerController, Score);
	DOREPLIFETIME(AFPSPlayerController, Kills);
	DOREPLIFETIME(AFPSPlayerController, Deaths);
	DOREPLIFETIME(AFPSPlayerController, Assists);
	DOREPLIFETIME(AFPSPlayerController, TeamScorePercent);
	DOREPLIFETIME(AFPSPlayerController, MyTeamScore);
	DOREPLIFETIME(AFPSPlayerController, RivalScorePercent);
	DOREPLIFETIME(AFPSPlayerController, RivalTeamScore);



}

void AFPSPlayerController::BeginPlay()
{
	Super::BeginPlay();
	MyPlayerState = Cast<AFPSPlayerState>(PlayerState);
	if (MyPlayerState)
	{
		UE_LOG(LogClass, Log, TEXT("MyPlayerState Created"));
	}
}
void AFPSPlayerController::ChangeKills(int32 delta)
{
	Kills += delta;
}
void AFPSPlayerController::ChangeScore(int32 delta)
{
	Score += delta;
}

void AFPSPlayerController::ChangeDeaths(int32 delta)
{
	Deaths += delta;
}

int32 AFPSPlayerController::GetDeaths()
{
	return Deaths;
}

void AFPSPlayerController::SetMyCharacter(AFPSCharacter* newCharacter, AFPSPlayerState* ps)
{
	if (AFPSPlayerState* myPS = Cast<AFPSPlayerState>(PlayerState))
	{
		if (myPS == ps)
		{
			
			if (Role == ROLE_Authority)
			{
				
				myPS->SetMyCharacter(newCharacter);
			}
			else
			{
				
				myPS->ServerSetMyCharacter(newCharacter);
			}
		}
		else
		{
			
		}
	}
}

int32 AFPSPlayerController::GetScore()
{
	return Score;
}
int32 AFPSPlayerController::GetKills()
{
	return Kills;
}

void AFPSPlayerController::Respawn()
{
	if (AcknowledgedPawn != NULL)
	{
		AcknowledgedPawn->Destroy();
	}
	UnPossess();
	AGameModeBase * GameMode = GetWorld()->GetAuthGameMode();
	if (GameMode)
	{
		//ChangeState(NAME_Playing);

		ServerRespawnPlayer();
	}

}


void AFPSPlayerController::ServerSetPlayerTeam_Implementation(int32 NewTeam)
{
	//TeamNumber = NewTeam;
	ServerSpawnPlayer();
}

bool AFPSPlayerController::ServerSetPlayerTeam_Validate(int32 NewTeam)
{
	return true;
}

void AFPSPlayerController::ServerSpawnPlayer_Implementation()
{
	// Spawn the new player
	AFPSProjectGameModeBase * GameMode = Cast<AFPSProjectGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->StartNewPlayer(this);
	}

}

bool AFPSPlayerController::ServerSpawnPlayer_Validate()
{
	return true;
}
void AFPSPlayerController::ServerRespawnPlayer_Implementation()
{
	// Spawn the new player
	AFPSProjectGameModeBase * GameMode = Cast<AFPSProjectGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->RespawnPlayer(this);
	}

}

bool AFPSPlayerController::ServerRespawnPlayer_Validate()
{
	return true;
}


void AFPSPlayerController::ServerSetPlayerTeamClient_Implementation(int32 NewTeam)
{
	//TeamNumber = NewTeam;
	ServerSpawnPlayerClient();
}

bool AFPSPlayerController::ServerSetPlayerTeamClient_Validate(int32 NewTeam)
{
	return true;
}

void AFPSPlayerController::ServerSpawnPlayerClient_Implementation()
{
	// Spawn the new player
	AFPSProjectGameModeBase * GameMode = Cast<AFPSProjectGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->StartNewPlayerClient(this);
	}

}

bool AFPSPlayerController::ServerSpawnPlayerClient_Validate()
{
	return true;
}


int32 AFPSPlayerController::GetPlayerTeam()
{
	return TeamNumber;
}

void AFPSPlayerController::TriggerAddAliveUI_Implementation()
{
	TriggerAddAliveUIBlueprint();

}
bool AFPSPlayerController::TriggerAddAliveUI_Validate() {
	return true;
}
void AFPSPlayerController::TriggerAddAliveUIBlueprint_Implementation()
{

}
void AFPSPlayerController::AddScoreBoardUI_Implementation()
{
	AddScoreBoardUIBlueprint();

}
bool AFPSPlayerController::AddScoreBoardUI_Validate() {
	return true;
}
void AFPSPlayerController::AddScoreBoardUIBlueprint_Implementation()
{

}

void AFPSPlayerController::UpdateScoreBoardUI_Implementation()
{
	UpdateScoreBoardUIBlueprint();

}
bool AFPSPlayerController::UpdateScoreBoardUI_Validate() {
	return true;
}
void AFPSPlayerController::UpdateScoreBoardUIBlueprint_Implementation()
{

}
void AFPSPlayerController::UpdateScoreUI_Implementation()
{
	UpdateScoreUIBlueprint();

}
bool AFPSPlayerController::UpdateScoreUI_Validate() {
	return true;
}
void AFPSPlayerController::UpdateScoreUIBlueprint_Implementation()
{

}

void AFPSPlayerController::Tick(float DeltaTime)
{
	UpdateScoreUI();

}

bool AFPSPlayerController::UpdateRivalStats_Validate(int32 RivalTeam, int32 RivalScore, float RivalPercent)
{
	return true;
}
void AFPSPlayerController::SetRivalTeam_Implementation(class ABaseTeam* NewRival)
{
	RivalTeam = NewRival;
}
void AFPSPlayerController::UpdateRivalStats_Implementation(int32 RivalTeam, int32 RivalScore, float RivalPercent)
{
	//RivalTeamNumber = RivalTeam;
	RivalTeamScore = RivalScore;
	RivalScorePercent = RivalPercent;

}
bool AFPSPlayerController::UpdateMyTeamStats_Validate(int32 NewScore, float MyPercent)
{
	return true;
}

void AFPSPlayerController::UpdateMyTeamStats_Implementation(int32 NewScore, float MyPercent)
{
	MyTeamScore = NewScore;
	TeamScorePercent = MyPercent;


}