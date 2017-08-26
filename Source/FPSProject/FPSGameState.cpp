#include "FPSGameState.h"
#include "Net/UnrealNetwork.h"

AFPSGameState::AFPSGameState()
{
	CurrentState = EGamePlayState::EUnknown;
	KillFeedMessage = TEXT("WHATSUPP");
	QueueTimerTime = 2.0f;
	NumberOfTeams = 0;
	TeamScores.Add(Team1Score);
	TeamScores.Add(Team2Score);

}
void AFPSGameState::OnRep_NumberOfTeams()
{

}
void AFPSGameState::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	check(World);
	CurrentState = EGamePlayState::EWaiting;
	if (CurrentState == EGamePlayState::EPlaying)
	{

	}
	switch (GetCurrentState())
	{
	case EGamePlayState::EWaiting:
		if (Role == ROLE_Authority)
		{
			GetWorldTimerManager().SetTimer(QueueTimer, this, &AFPSGameState::ServerAllowTeamChoice, QueueTimerTime, false);
		}
		break;
	case EGamePlayState::EPlaying:

		//start draining power
		GetWorldTimerManager().SetTimer(UpdateTimer, this, &AFPSGameState::Update, UpdateDelay, true);
		break;
	case EGamePlayState::EGameOver:

		//stop draining power
		//GetWorldTimerManager().ClearTimer(UpdateTimer);
		break;
	default:
	case EGamePlayState::EUnknown:
		break;
	}


}

bool AFPSGameState::ServerAllowTeamChoice_Validate()
{
	return true;
}


void AFPSGameState::CallUpdateScoreboard()
{
	if (Team1PlayerOrder.Num() != 0 && Team2PlayerOrder.Num() != 0)
	{
		for (FConstControllerIterator controller = GetWorld()->GetControllerIterator(); controller; ++controller) {
			if (AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(*controller)) {
				//PlayerController->UpdateScoreBoardUI();
			}
		}
		/*
		for (int32 i = 0; i < Team2PlayerOrder.Num(); ++i)
		{
		if (AFPSPlayerController* playercontroller = Cast<AFPSPlayerController>(Team2PlayerOrder[i]))
		{
		playercontroller->UpdateScoreBoardUI();
		}
		}
		for (int32 it = 0; it < Team1PlayerOrder.Num(); ++it)
		{
		if (AFPSPlayerController* playercontroller = Cast<AFPSPlayerController>(Team1PlayerOrder[it]))
		{
		playercontroller->UpdateScoreBoardUI();
		}
		}
		*/
	}
}
void AFPSGameState::ServerAllowTeamChoice_Implementation()
{
	if (Role == ROLE_Authority)
	{
		BPAllowTeamChoice();
	}
}
void AFPSGameState::ClientUpdateScore_Implementation(int32 Team, int32 delta)
{
	if (Team == 1)
	{
		Team1Score += delta;
	}
	if (Team == 2)
	{
		Team2Score += delta;
	}
}
void AFPSGameState::ClientUpdateKillFeedMessage_Implementation(FName text)
{

	KillFeedMessage = text.ToString();

}
bool AFPSGameState::ClientUpdateNumberOfTeams_Validate(int32 Teams)
{
	return true;
}

void AFPSGameState::ClientUpdateNumberOfTeams_Implementation(int32 Teams)
{

	NumberOfTeams = Teams;

}
bool AFPSGameState::UpdateTeamList_Validate(const TArray<ABaseTeam*>& NewTeamList)
{
	return true;
}
void AFPSGameState::UpdateTeamList_Implementation(const TArray<ABaseTeam*>& NewTeamList)
{
	Teams = NewTeamList;
}




bool AFPSGameState::UpdateTeamOrder_Validate(const TArray<AFPSPlayerState*>& Team1Order, const TArray<AFPSPlayerState*>& Team2Order)
{
	return true;
}
void AFPSGameState::UpdateTeamOrder_Implementation(const TArray<AFPSPlayerState*>& Team1Order, const TArray<AFPSPlayerState*>& Team2Order)
{
	Team1PlayerOrder.Empty();
	Team2PlayerOrder.Empty();

	Team1PlayerOrder = Team1Order;
	Team2PlayerOrder = Team2Order;
	//Team1PlayerObjects = *Team1PlayerOrder;
	//Team2PlayerObjects = *Team2PlayerOrder;
	UE_LOG(LogClass, Log, TEXT("Team1PlayerOrderNUM: %d"), Team1PlayerOrder.Num());
	UE_LOG(LogClass, Log, TEXT("Team2PlayerOrderNUM: %d"), Team2PlayerOrder.Num());
	if (Team1PlayerOrder.IsValidIndex(0))
	{
		if (AFPSPlayerState* indexcontroller = Cast<AFPSPlayerState>(Team1PlayerOrder[0]))
		{
			//UE_LOG(LogClass, Log, TEXT("HI THERE TREY"));
			/*
			if (indexcontroller->GetPawn() != NULL) {
			//UE_LOG(LogClass, Log, TEXT("Team1FirstCharacter: %s"), *indexcontroller->GetPawn()->GetName());
			}
			UE_LOG(LogClass, Log, TEXT("Team1FirstPlayer: %s"), *FString::FromInt(UGameplayStatics::GetPlayerControllerID(indexcontroller)));
			*/
		}
	}

	//testcontroller = &controller;
	if (AFPSPlayerController* pc = Cast<AFPSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		pc->UpdateScoreBoardUI();
	}

}

bool AFPSGameState::ClientUpdateTeam1PlayerStates_Validate(AFPSPlayerState* PlayerState)
{
	return true;
}
void AFPSGameState::ClientUpdateTeam1PlayerStates_Implementation(AFPSPlayerState* PlayerState)
{
	Team1PlayerStates.Add(PlayerState);
}
bool AFPSGameState::ClientUpdateTeam2PlayerStates_Validate(AFPSPlayerState* PlayerState)
{
	return true;
}
void AFPSGameState::ClientUpdateTeam2PlayerStates_Implementation(AFPSPlayerState* PlayerState)
{
	Team2PlayerStates.Add(PlayerState);
}

bool AFPSGameState::ClientUpdateTeam1Controllers_Validate(AFPSPlayerController* FPSController)
{
	return true;
}
void AFPSGameState::ClientUpdateTeam1Controllers_Implementation(AFPSPlayerController* FPSController)
{
	Team1PlayerControllers.Add(FPSController);
	//Team1PlayerControllers = FPSControllers;
	UE_LOG(LogClass, Log, TEXT("Team 1 has %d controllers"), Team1PlayerControllers.Num());
}




void AFPSGameState::SetNumberOfPlayers_Implementation(int32 newNumberOfPlayers)
{
	NumberOfPlayers = newNumberOfPlayers;
}
bool AFPSGameState::ClientUpdateTeam2Controllers_Validate(AFPSPlayerController* FPSController)
{
	return true;
}
void AFPSGameState::ClientUpdateTeam2Controllers_Implementation(AFPSPlayerController* FPSController)
{
	Team2PlayerControllers.Add(FPSController);
	UE_LOG(LogClass, Log, TEXT("Team 2 has %d controllers"), Team1PlayerControllers.Num());
}

void AFPSGameState::ClientUpdateWinningTeam_Implementation(int32 team, FName WinningText)
{
	WinningTeam = team;
	WinningMessage = WinningText.ToString();
}

void AFPSGameState::Update()
{
	UWorld* World = GetWorld();
	check(World);







	if (GetCurrentState() == EGamePlayState::EPlaying)
	{

		//UE_LOG(LogClass, Log, TEXT("KILLED"));
		for (FConstControllerIterator It = World->GetControllerIterator(); It; ++It) {
			if (APlayerController* PlayerController = Cast<APlayerController>(*It)) {
				if (AFPSPlayerState* ps = Cast<AFPSPlayerState>(PlayerController->PlayerState))
				{
					if (ps->Betrayals >= 2)
					{
						UE_LOG(LogClass, Log, TEXT("Player Should Be Booted"));
					}
				}
				if (AFPSCharacter* Player = Cast<AFPSCharacter>(PlayerController->GetPawn())) {
					if (Player->GetCurrentHealth() <= 0.0f) {
						if (Player->Shooter == NULL) {
							Player->Shooter = Player;
						}
						//UE_LOG(LogClass, Log, TEXT("KILLED"));

						if (AFPSPlayerController* FPSController = Cast<AFPSPlayerController>(PlayerController))
						{
							FPSController->MyPlayerState->SetDeathsMultiCast(1);

							if (AFPSPlayerController* ShooterController = Cast<AFPSPlayerController>(Player->Shooter->Controller))
							{
								if (ShooterController->MyPlayerState)
								{
									if (ShooterController->MyPlayerState->Team != FPSController->MyPlayerState->Team)
									{
										

										ShooterController->MyPlayerState->SetKillsMultiCast(1);
										ShooterController->MyPlayerState->SetScoreMultiCast(10);
										LastKiller = ShooterController->MyPlayerState->UserName;
										LastKilled = FPSController->MyPlayerState->UserName;
										

										KillFeedMessage = TEXT("" + LastKiller + " killed " + LastKilled);//(TEXT("%s killed %f"), LastKiller, LastKilled);

										ClientUpdateKillFeedMessage(FName(*KillFeedMessage));

										//SetNewKillFeedMessage(KillFeedMessage);

										if (AFPSPlayerController* FPSController = Cast<AFPSPlayerController>(Player->Shooter->Controller))
										{
											if (AFPSPlayerState* ps = Cast<AFPSPlayerState>(FPSController->PlayerState))
											{

												if (ps->Team->TeamNumber != 0)
												{
													if (AFPSProjectGameModeBase* GameMode = Cast<AFPSProjectGameModeBase>(GetWorld()->GetAuthGameMode()))
													{
														ps->Team->ChangeScore(GameMode->PointsPerKill);
														/*
														if (ps->Team->TeamNumber == 1)
														{

														ClientUpdateScore(1, GameMode->PointsPerKill);
														UpdateScoreBlueprintEvent();

														}
														if (ps->Team->TeamNumber == 2)
														{
														ClientUpdateScore(2, GameMode->PointsPerKill);
														UpdateScoreBlueprintEvent();
														UE_LOG(LogClass, Log, TEXT("Team2 Scored"));

														}
														if (ps->Team->TeamNumber == 3)
														{
														ClientUpdateScore(3, GameMode->PointsPerKill);
														UpdateScoreBlueprintEvent();

														}
														if (ps->Team->TeamNumber == 4)
														{
														ClientUpdateScore(4, GameMode->PointsPerKill);
														UpdateScoreBlueprintEvent();

														}
														*/

													}

												}
											}
											//}


										}
									}
									else
									{
										LastKiller = ShooterController->MyPlayerState->UserName;
										LastKilled = FPSController->MyPlayerState->UserName;

										KillFeedMessage = TEXT("" + LastKiller + " betrayed " + LastKilled);
										ShooterController->MyPlayerState->Betrayals += 1;
										ClientUpdateKillFeedMessage(FName(*KillFeedMessage));
									}
								}

							}
						}



						////////////////////////////////////////////////////////////////////////////////////////////////

						/*
						TArray<AFPSPlayerState*> CorrectTeam1PlayerOrder;
						TArray<AFPSPlayerState*> CorrectTeam2PlayerOrder;
						if (Team1PlayerControllers.Num() + Team2PlayerControllers.Num() == NumberOfPlayers)
						{
							//Team2CorrectOrder.Empty();
							//UE_LOG(LogClass, Log, TEXT("Team2Controllers: %d"), Team2PlayerControllers.Num());
							int32 Team2BestScore = -1;

							for (int32 i = 0; i < Team2PlayerControllers.Num(); ++i)
							{

								if (AFPSPlayerState* ps = Cast<AFPSPlayerState>(Team2PlayerControllers[i]->PlayerState))
								{
									if (ps->GetScore() > Team2BestScore)
									{
										CorrectTeam2PlayerOrder.Insert(ps, 0);
										Team2BestScore = ps->GetScore();
									}
									else if (CorrectTeam2PlayerOrder.Num() > 1)
									{
										for (int32 something = 0; something < CorrectTeam2PlayerOrder.Num(); ++something)
										{
											if (CorrectTeam2PlayerOrder.Contains(ps) == false)
											{
												if (AFPSPlayerState* OtherPS = Cast<AFPSPlayerState>(CorrectTeam2PlayerOrder[something]))
												{
													if (ps->GetScore() > OtherPS->GetScore())
													{
														CorrectTeam2PlayerOrder.Insert(ps, something);
													}
												}
												else
												{
													CorrectTeam2PlayerOrder.Emplace(ps);
												}
											}
										}
										if (CorrectTeam2PlayerOrder.Contains(ps) == false)
										{
											CorrectTeam2PlayerOrder.Emplace(ps);
										}
									}
									else
									{
										if (CorrectTeam2PlayerOrder.Contains(ps) == false)
										{
											CorrectTeam2PlayerOrder.Emplace(ps);

										}
									}


							int32 Team1BestScore = -1;
							UE_LOG(LogClass, Log, TEXT("Team1PlayerControllersNUM: %d"), Team1PlayerControllers.Num());
							UE_LOG(LogClass, Log, TEXT("Team1PlayerStatesNUM: %d"), Team1PlayerStates.Num());
							for (int32 spot = 0; spot < Team1PlayerControllers.Num(); ++spot)
							{

								if (AFPSPlayerState* ps2 = Cast<AFPSPlayerState>(Team1PlayerControllers[spot]->PlayerState))
								{
									if (ps2->GetScore() > Team1BestScore)
									{
										UE_LOG(LogClass, Log, TEXT("Team1TopPlayer"));
										CorrectTeam1PlayerOrder.Insert(ps2, 0);
										Team1BestScore = ps2->GetScore();
									}
									else if (CorrectTeam1PlayerOrder.Num() > 1)
									{
										for (int32 something2 = 0; something2 < CorrectTeam1PlayerOrder.Num(); ++something2)
										{
											if (CorrectTeam1PlayerOrder.Contains(ps2) == false)
											{
												if (AFPSPlayerState* OtherPS2 = Cast<AFPSPlayerState>(CorrectTeam1PlayerOrder[something2]))
												{
													if (ps2->GetScore() > OtherPS2->GetScore())
													{
														UE_LOG(LogClass, Log, TEXT("Team1FoundWorsePlayer"));
														CorrectTeam1PlayerOrder.Insert(ps2, something2);
													}
												}
												else
												{
													CorrectTeam1PlayerOrder.Emplace(ps2);
												}
											}
										}
										if (CorrectTeam1PlayerOrder.Contains(ps2) == false)
										{
											CorrectTeam1PlayerOrder.Emplace(ps2);
										}

										
									}
									else
									{
										if (CorrectTeam1PlayerOrder.Contains(ps2) == false)
										{
											CorrectTeam1PlayerOrder.Emplace(ps2);
											UE_LOG(LogClass, Log, TEXT("added to end"));

										}
									}
									


								}
								else
								{

								}


							}
							UE_LOG(LogClass, Log, TEXT("CorrectTeam1PlayerOrderNUM: %d"), CorrectTeam1PlayerOrder.Num());
							if (CorrectTeam2PlayerOrder.Num() == Team2PlayerControllers.Num())
							{
								Team2PlayerOrder = CorrectTeam2PlayerOrder;
							}
							if (CorrectTeam1PlayerOrder.Num() == Team1PlayerControllers.Num())
							{
								UE_LOG(LogClass, Log, TEXT("AYYYYYY"));
								Team1PlayerOrder = CorrectTeam1PlayerOrder;


							}
							else
							{

								UE_LOG(LogClass, Log, TEXT("NOOOOO"));
							}




							UpdateTeamOrder(Team1PlayerOrder, Team2PlayerOrder);



						}
						else
						{
							UE_LOG(LogClass, Log, TEXT("NOTSIXPLAYERSYET "));
						}


						*/














						CallHUDUpdate();

						Player->OnPlayerDeath();
	


						










					}

				}
			}


			if (AFPSPlayerController* FPSController = Cast<AFPSPlayerController>(*It))
			{

				if (AFPSProjectGameModeBase* GameMode = Cast<AFPSProjectGameModeBase>(GetWorld()->GetAuthGameMode()))
				{
					if (AFPSPlayerState* ps = Cast<AFPSPlayerState>(FPSController->PlayerState))
					{
						
	
						float ScorePercent = ((double)ps->Team->TeamScore / GameMode->ScoreToWin);

						FPSController->UpdateMyTeamStats(ps->Team->TeamScore, ScorePercent);

				

					}

				}
				AFPSPlayerController* MyRival = NULL;
				int32 RivalDifference = 100000;
				for (FConstControllerIterator OtherControllers = World->GetControllerIterator(); OtherControllers; ++OtherControllers) {
					if (AFPSPlayerController* OtherController = Cast<AFPSPlayerController>(*OtherControllers))
					{
						if (OtherController != FPSController)
						{
							if (AFPSPlayerState* ps = Cast<AFPSPlayerState>(FPSController->PlayerState))
							{
								if (AFPSPlayerState* ps2 = Cast<AFPSPlayerState>(OtherController->PlayerState))
								{
									if (ps2->Team->TeamNumber != ps->Team->TeamNumber)
									{


										if (ps2->Team->TeamNumber != 0)
										{
											
											if (FGenericPlatformMath::Abs(ps2->Team->TeamScore - ps->Team->TeamScore) < RivalDifference)
											{
												MyRival = OtherController;
												RivalDifference = (FGenericPlatformMath::Abs(ps2->Team->TeamScore - ps->Team->TeamScore));
											}
											
										}
									}
								}
							}
						}
					}
				}
				
				if (MyRival != NULL)
				{
					if (AFPSPlayerState* Rivalps = Cast<AFPSPlayerState>(MyRival->PlayerState))
					{
						if (AFPSProjectGameModeBase* GameMode = Cast<AFPSProjectGameModeBase>(GetWorld()->GetAuthGameMode())) {

					
							float RivalScorePercent = ((double)Rivalps->Team->TeamScore / GameMode->ScoreToWin);

							FPSController->UpdateRivalStats(Rivalps->Team->TeamNumber, Rivalps->Team->TeamScore, RivalScorePercent);
							if (Rivalps->Team != NULL)
							{
								if (AFPSPlayerState* Myps = Cast<AFPSPlayerState>(FPSController->PlayerState))
								{
									Myps->SetRivalTeam(Rivalps->Team);
								}
							}
							
						}
					}
				}
			}

		}
	}
	if (AFPSProjectGameModeBase* GameMode = Cast<AFPSProjectGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		for (int32 i = 0; i < Teams.Num(); ++i)
		{
			if (ABaseTeam* team = Cast<ABaseTeam>(Teams[i]))
			{
				if (team->TeamScore >= GameMode->ScoreToWin)
				{
					WinningTeam = team->TeamNumber;
					WinningMessage = TEXT("%s Won"),*team->TeamName.ToString();
					ClientUpdateWinningTeam(WinningTeam, FName(*WinningMessage));
					CallHUDGameOver();
					for (FConstControllerIterator It = World->GetControllerIterator(); It; ++It) {
						if (AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(*It)) {
							if (AFPSCharacter* Player = Cast<AFPSCharacter>(PlayerController->GetPawn())) {
								Player->SetCurrentState(EPlayerState::EPlayerWaiting);
							}

						}

					}
					SetCurrentState(EGamePlayState::EGameOver);
				}
			}
		}

	}

}


int32 AFPSGameState::GetTeam1Score()
{
	return Team1Score;
}
int32 AFPSGameState::GetTeam2Score()
{
	return Team2Score;
}



void AFPSGameState::SetNewKillFeedMessage(FString Message)
{

	KillFeedMessage = TEXT("Something");

	/*
	if (APlayerHUD* HUD = Cast<APlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD())) {
	HUD->AddMessageEvent();
	}
	*/
}

void AFPSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFPSGameState, KillFeedMessage);
	DOREPLIFETIME(AFPSGameState, Teams);
	DOREPLIFETIME(AFPSGameState, testplayerstate);
	DOREPLIFETIME(AFPSGameState, CurrentState);
	DOREPLIFETIME(AFPSGameState, WinningPlayerName);
	DOREPLIFETIME(AFPSGameState, Team2PlayerStates);
	DOREPLIFETIME(AFPSGameState, Team1PlayerStates);
	DOREPLIFETIME(AFPSGameState, Team2PlayerControllers);
	DOREPLIFETIME(AFPSGameState, Team1PlayerControllers);
	DOREPLIFETIME(AFPSGameState, Team2PlayerOrder);
	DOREPLIFETIME(AFPSGameState, Team1PlayerOrder);

	DOREPLIFETIME(AFPSGameState, Team2PlayerObjects);
	DOREPLIFETIME(AFPSGameState, Team1PlayerObjects);

	DOREPLIFETIME(AFPSGameState, Team2CorrectOrder);
	DOREPLIFETIME(AFPSGameState, Team1CorrectOrder);

}

EGamePlayState AFPSGameState::GetCurrentState() const
{
	return CurrentState;
}

void AFPSGameState::SetCurrentState(EGamePlayState NewState)
{
	if (Role == ROLE_Authority) {
		CurrentState = NewState;
		if (CurrentState == EGamePlayState::EPlaying)
		{

			GetWorldTimerManager().SetTimer(UpdateTimer, this, &AFPSGameState::Update, UpdateDelay, true);
		}
		if (CurrentState == EGamePlayState::EGameOver)
		{
			//GetWorldTimerManager().ClearTimer(UpdateTimer);
		}
	}
}

void AFPSGameState::OnRep_CurrentState()
{

}

bool AFPSGameState::UpdateTeamScore_Validate(int32 Team)
{
	return true;
}

void AFPSGameState::UpdateTeamScore_Implementation(int32 Team)
{
	/*
	UE_LOG(LogClass, Log, TEXT("Reached Function call"));
	if (Role == ROLE_Authority)
	{
	UE_LOG(LogClass, Log, TEXT("SERVER"));
	if (AFPSProjectGameModeBase* GameMode = Cast<AFPSProjectGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
	UE_LOG(LogClass, Log, TEXT("CASTED"));
	if (Team == 1)
	{
	int32 NewScore = Team1Score + GameMode->PointsPerKill;
	ClientUpdateScore(1, NewScore);
	Team1Score = NewScore;
	UpdateScoreBlueprintEvent();
	UE_LOG(LogClass, Log, TEXT("UPDATESCORE"));
	}
	}

	}
	*/
}




