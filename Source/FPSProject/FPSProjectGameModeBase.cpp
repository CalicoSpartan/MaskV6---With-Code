// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSProjectGameModeBase.h"
#include "FPSCharacter.h"
#include "PlayerHUD.h"
#include "UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/GameFramework/GameMode.h"
#include "FPSPlayerState.h"


#include "Engine.h"

AFPSProjectGameModeBase::AFPSProjectGameModeBase()
{

	/*
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_FPSCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set the type of HUD used in the game
	static ConstructorHelpers::FClassFinder<AHUD> PlayerHUDClass(TEXT("/Game/Blueprints/BP_PlayerHUD"));
	if (PlayerHUDClass.Class != NULL) {
		HUDClass = PlayerHUDClass.Class;
	}
	*/

	Team1Players = 0;
	Team2Players = 0;
	Team3Players = 0;
	Team4Players = 0;
	Team5Players = 0;
	SafeSpawnDistance = 1500;
	HasLoggedIn = false;
	NumTeamA = 0;
	NumTeamB = 0;
	SpawnDelay = 2;
	PointsPerKill = 1;

	AddKillFeedEntry = false;
	//set the type of gamestate used in the game
	GameStateClass = AFPSGameState::StaticClass();

	PlayerControllerClass = AFPSPlayerController::StaticClass();

	PlayerStateClass = AFPSPlayerState::StaticClass();
	NumberOfTeams = 2;
	ScoreToWin = 6;
	
}

void AFPSProjectGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFPSProjectGameModeBase::RespawnPlayer(APlayerController* NewPlayer)
{
	UE_LOG(LogClass, Log, TEXT("Respawning"));
	if (AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(NewPlayer))
	{
		if (AFPSPlayerState* PlayerState = Cast<AFPSPlayerState>(PlayerController->PlayerState))
		{
			//TArray<AFPSPlayerStart*> EnemyPreferredStarts;
			//TArray<AFPSPlayerStart*> TeammatePreferredStarts;
			TArray<AFPSPlayerStart*> TeammateStarts;
			FVector2D ClosestTeammateFarthestEnemy;
			float FarthestDistance = -1.0f;
			float ClosestDistToMate = 100000.0f;
			bool checkforsafety = true;
			TArray<AFPSPlayerStart*> StartsArray;
			TArray<float> StartsDistFromEnemy;
			TArray<float> StartsDistFromMate;
			TArray<float> StartsDistFromMateInOrder;
			int32 iteratorindex = 0;
			if (AFPSGameState* gameState = Cast<AFPSGameState>(GameState))
			{
				if (gameState->GetCurrentState() != EGamePlayState::EGameOver)
				{



					for (TActorIterator<AFPSPlayerStart> PlayerStart(GetWorld()); PlayerStart; ++PlayerStart)
					{
						if (PlayerStart->Tags.Contains("RespawnPoint"))
						{
							float myFarthestDistanceFromEnemy = 0.0f;
							float myClosestDistanceFromMate = 5000000.0f;
							for (FConstControllerIterator Iterator = GetWorld()->GetControllerIterator(); Iterator; ++Iterator)
							{
								if (AFPSPlayerController* aPlayerController = Cast<AFPSPlayerController>(*Iterator))
								{
									if (aPlayerController != PlayerController)
									{
										if (AFPSPlayerState* aPlayerState = Cast<AFPSPlayerState>(aPlayerController->PlayerState))
										{
											if (PlayerState->Team != aPlayerState->Team)
											{
												if (AFPSCharacter* aCharacter = Cast<AFPSCharacter>(aPlayerController->GetPawn()))
												{
													float dist = (aCharacter->GetActorLocation() - PlayerStart->GetActorLocation()).Size();
													if (dist > PlayerStart->FarthestDistanceFromEnemy)
													{
														PlayerStart->FarthestDistanceFromEnemy = dist;
													}
													if (dist > myFarthestDistanceFromEnemy)
													{
														myFarthestDistanceFromEnemy = dist;
													}

													if (dist > FarthestDistance)
													{
															
														//EnemyPreferredStarts.Insert(*PlayerStart, 0);
														FarthestDistance = dist;

													}
													else
													{
														/*
														
														if (EnemyPreferredStarts.Num() > 1)
														{
															for (int32 index = 0; index < EnemyPreferredStarts.Num(); index++)
															{
																if (!EnemyPreferredStarts.Contains(*PlayerStart))
																{
																	if ((EnemyPreferredStarts[index]->GetActorLocation() - aCharacter->GetActorLocation()).Size() < dist)
																	{
																		EnemyPreferredStarts.Insert(*PlayerStart, index);
																	}
																}
															}
														}
														if (!EnemyPreferredStarts.Contains(*PlayerStart))
														{
															EnemyPreferredStarts.Emplace(*PlayerStart);
														}
														*/
														

													}
												}
											}
											else
											{
												if (AFPSCharacter* aCharacter = Cast<AFPSCharacter>(aPlayerController->GetPawn()))
												{
													float dist = (aCharacter->GetActorLocation() - PlayerStart->GetActorLocation()).Size();
													if (dist < PlayerStart->ClosestDistanceToTeammate)
													{
														PlayerStart->ClosestDistanceToTeammate = dist;
													}
													if (dist < myClosestDistanceFromMate)
													{
														myClosestDistanceFromMate = dist;
													}
													
													if (TeammateStarts.Num() > 0)
													{
														for (int32 i = 0; i < TeammateStarts.Num(); i++)
														{
															if (!TeammateStarts.Contains(*PlayerStart))
															{
																if (AFPSPlayerStart* otherPlayerStart = Cast<AFPSPlayerStart>(TeammateStarts[i]))
																{
																	if (myClosestDistanceFromMate < StartsDistFromMateInOrder[i])
																	{
																		TeammateStarts.Insert(*PlayerStart, i);
																		StartsDistFromMateInOrder.Insert(myClosestDistanceFromMate, i);
																		break;
																	}

																}
															}
														}
														if (!TeammateStarts.Contains(*PlayerStart))
														{
															TeammateStarts.Emplace(*PlayerStart);
															StartsDistFromMateInOrder.Emplace(myClosestDistanceFromMate);
														}
													}
													else
													{
														StartsDistFromMateInOrder.Emplace(myClosestDistanceFromMate);
														TeammateStarts.Emplace(*PlayerStart);
													}
													
													if (dist < ClosestDistToMate)
													{

														//TeammatePreferredStarts.Insert(*PlayerStart, 0);
														ClosestDistToMate = dist;

													}
												}
											}
										}
									}
								}


							}
							StartsDistFromMate.Emplace(myClosestDistanceFromMate);
							StartsDistFromEnemy.Emplace(myFarthestDistanceFromEnemy);
							StartsArray.Emplace(*PlayerStart);


						}
						iteratorindex += 1;

					}
					if (StartsDistFromMate.Num() > 1)
					{
						float Average = 0.0f;
						for (int32 i = 0; i < StartsDistFromMate.Num(); i++)
						{
							Average += StartsDistFromMate[i];
						}
						Average = Average / StartsDistFromMate.Num();
						float BelowAverage = 0.0f;
						if (StartsDistFromMateInOrder[StartsDistFromMateInOrder.Num() - 1])
						{
							BelowAverage = (Average + StartsDistFromMateInOrder[StartsDistFromMateInOrder.Num() - 1]) / 2.0f;
						}
						ClosestDistToMate = BelowAverage;
						
					}
					ClosestTeammateFarthestEnemy = FVector2D(ClosestDistToMate, FarthestDistance);
					float BestMagnitude = 3000000000.0f;
					float testFarthestDistance = 30000000.0f;
					AFPSPlayerStart* BestStart = NULL;

					for (int32 i = 0; i < StartsArray.Num(); i++)
					{
						if (AFPSPlayerStart* playerStart = Cast<AFPSPlayerStart>(StartsArray[i]))
						{
							
							FVector2D myCordinate = FVector2D(StartsDistFromMate[i], StartsDistFromEnemy[i]);
							float myDistanceToBest = (ClosestTeammateFarthestEnemy - myCordinate).Size();
							if (myDistanceToBest < BestMagnitude)
							{
								BestStart = playerStart;
								BestMagnitude = myDistanceToBest;
							}
							UE_LOG(LogClass, Log, TEXT("This is %s and my magnitude is %f"), *playerStart->GetName(), myDistanceToBest);
						}
					}
					/*
					for (TActorIterator<AFPSPlayerStart> PlayerStart(GetWorld()); PlayerStart; ++PlayerStart)
					{
						if (PlayerStart->Tags.Contains("RespawnPoint"))
						{
							FVector2D myCordinate = FVector2D(PlayerStart->ClosestDistanceToTeammate, PlayerStart->FarthestDistanceFromEnemy);
							float myDistanceToBest = (ClosestTeammateFarthestEnemy - myCordinate).Size();
							UE_LOG(LogClass, Log, TEXT("This is %s my distance to best possible point is %f"), *PlayerStart->GetName(), myDistanceToBest);
							/*
							if (abs(myCordinate.Y - ClosestTeammateFarthestEnemy.Y) < testFarthestDistance)
							{
								BestStart = *PlayerStart;
								testFarthestDistance = abs(myCordinate.Y - ClosestTeammateFarthestEnemy.Y);
							}
							*//*
							if (myDistanceToBest < BestMagnitude)
							{
								BestStart = *PlayerStart;
								BestMagnitude = myDistanceToBest;
							}
							
						}
					}
					*/
					if (BestStart != NULL)
					{
						UE_LOG(LogClass, Log, TEXT("The Best start is: %s with a magnitude of %f"), *BestStart->GetName(),BestMagnitude);
						NewPlayer->SetPawn(SpawnDefaultPawnFor(NewPlayer, BestStart));
						RestartPlayer(NewPlayer);
						AFPSCharacter* Character = Cast<AFPSCharacter>(NewPlayer->GetPawn());
						if (Character)
						{
							Character->TriggerAddUI();
							Character->AddTeamColor();

						}
					}
					else
					{
						UE_LOG(LogClass, Log, TEXT("No Best Start"));
					}



					/*
					
					if (PreferredStarts.Num() > 1) {
						AFPSPlayerStart* BestStart = PreferredStarts[FMath::RandRange(0, PreferredStarts.Num() - 1)];
						float SpawnDistance = 0.0f;
						for (int32 i = 0; i < PreferredStarts.Num(); ++i) {
							for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
							{

								if (AFPSPlayerController* newPlayerController = Cast<AFPSPlayerController>(*Iterator))
								{
									if (newPlayerController != PlayerController) {

										if (AFPSPlayerState* playerstate = Cast<AFPSPlayerState>(PlayerController->PlayerState)) {
											if (AFPSPlayerState* newplayerstate = Cast<AFPSPlayerState>(newPlayerController->PlayerState)) {
												if (playerstate->Team != newplayerstate->Team)
												{


													if (AFPSCharacter* newPlayerCharacter = Cast<AFPSCharacter>(newPlayerController->GetPawn()))
													{
														if (UCameraComponent* newPlayerCamera = Cast<UCameraComponent>(newPlayerCharacter->FPSCameraComponent))
														{
															FVector CameraLocation = newPlayerCamera->GetComponentLocation();
															FVector SpawnPointLocation = PreferredStarts[i]->GetActorLocation();
															if (FVector::Dist(CameraLocation, SpawnPointLocation) > SpawnDistance)
															{
																BestStart = PreferredStarts[i];
																SpawnDistance = FVector::Dist(CameraLocation, SpawnPointLocation);
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
						int32 PlayerStartIndex = FMath::RandRange(0, PreferredStarts.Num() - 1);
						NewPlayer->SetPawn(SpawnDefaultPawnFor(NewPlayer, BestStart));

						RestartPlayer(NewPlayer);
						AFPSCharacter* Character = Cast<AFPSCharacter>(NewPlayer->GetPawn());
						if (Character)
						{
							Character->TriggerAddUI();
							Character->AddTeamColor();

						}
						if (PlayerController)
						{

						}
					}
					else {
						int32 PlayerStartIndex = 0;
						NewPlayer->SetPawn(SpawnDefaultPawnFor(NewPlayer, PreferredStarts[PlayerStartIndex]));

						RestartPlayer(NewPlayer);
						AFPSCharacter* Character = Cast<AFPSCharacter>(NewPlayer->GetPawn());
						if (Character)
						{
							Character->TriggerAddUI();

						}
						if (PlayerController)
						{

						}
					}
					*/
				}
			}
		}
	}

}



void AFPSProjectGameModeBase::StartNewPlayerClient(APlayerController* NewPlayer)
{
	if (AFPSPlayerState* NewPlayerState = Cast<AFPSPlayerState>(NewPlayer->PlayerState)) {
		if (NewPlayerState->Team != NULL)
		{

			


			if (AFPSGameState* MyGameState = Cast<AFPSGameState>(GameState))
			{
				MyGameState->SetNumberOfPlayers(GetNumPlayers());
			}
			NewPlayer->GetPawn()->Destroy();
			AFPSPlayerController* TestPlayerController = Cast<AFPSPlayerController>(NewPlayer);
			TArray<AFPSPlayerStart*> PreferredStarts;
			UE_LOG(LogClass, Log, TEXT("Client: My starting team number is: %d"), NewPlayerState->Team->TeamNumber);
			bool BlockCheck = true;
			for (int32 i = 0; i < 2; ++i) {

				for (TActorIterator<AFPSPlayerStart> PlayerStart(GetWorld()); PlayerStart; ++PlayerStart)
				{

					if (TestPlayerController)
					{
						

						if (NewPlayerState->Team->TeamNumber == PlayerStart->TeamNumber && NewPlayerState->Team->TeamNumber != 0) //&& PlayerStart->PlayerStartTag != FName(TEXT("Blocked")))
						{
							if (PlayerStart->PlayerStartTag == FName(TEXT("Blocked"))) {
								if (BlockCheck == false) {

									PreferredStarts.AddUnique(*PlayerStart);
								}
								else {

								}

							}
							else
							{

								PreferredStarts.AddUnique(*PlayerStart);
							}
							// Player should spawn on CT.


						}
						

					}

				}
				if (PreferredStarts.Num() == 0) {
					BlockCheck = false;
				}
				else {
					break;
				}

			}

			if (PreferredStarts.Num() > 0)
			{

				

				//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Player has selected a Team");
				int32 PlayerStartIndex = FMath::RandRange(0, PreferredStarts.Num() - 1);
				NewPlayer->SetPawn(SpawnDefaultPawnFor(NewPlayer, PreferredStarts[PlayerStartIndex]));
				PreferredStarts[PlayerStartIndex]->PlayerStartTag = FName(TEXT("Blocked"));

				RestartPlayer(NewPlayer);
				AFPSCharacter* Character = Cast<AFPSCharacter>(NewPlayer->GetPawn());
				if (Character)
				{
					Character->TriggerAddUI();
					Character->AddTeamColor();

				}

				if (TestPlayerController)
				{
					if (AFPSGameState* MyGameState = Cast<AFPSGameState>(GameState))
					{
						MyGameState->ClientUpdateNumberOfTeams(NumberOfTeams);
						
					}
					
				}
				UE_LOG(LogClass, Log, TEXT("client controller has team and spawned in"));
			}
			

		}
	}
	

}


void AFPSProjectGameModeBase::StartNewPlayer(APlayerController* NewPlayer)
{

	
	if (AFPSPlayerController* TestPlayerController = Cast<AFPSPlayerController>(NewPlayer))
	{
		if (AFPSPlayerState* MyPlayerState = Cast<AFPSPlayerState>(TestPlayerController->PlayerState))
		{
			if (MyPlayerState->Team == NULL)
			{
				Team1Players++;
				if (ABaseTeam* Team1 = Cast<ABaseTeam>(Teams[1]))
				{
					//Team1->TeamNumber = 1;
					Team1->AddToPlayerStates(MyPlayerState);
					MyPlayerState->SetTeam(Team1);
					
				}
			}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			
			TArray<AFPSPlayerStart*> PreferredStarts;
			if (AFPSGameState* MyGameState = Cast<AFPSGameState>(GameState))
			{
				MyGameState->SetNumberOfPlayers(GetNumPlayers());
			}
			UE_LOG(LogClass, Log, TEXT("Server: My starting team number is: %d"), MyPlayerState->Team->TeamNumber);

			for (TActorIterator<AFPSPlayerStart> PlayerStart(GetWorld()); PlayerStart; ++PlayerStart)
			{




				if (MyPlayerState->Team->TeamNumber == PlayerStart->TeamNumber)
				{

					// Player should spawn on CT.
					PreferredStarts.Add(*PlayerStart);

				}

			
			}
			
			
			if (AFPSGameState* MyGameState = Cast<AFPSGameState>(GameState))
			{


				for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
				{
					AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(*Iterator);
					if (PlayerController) {
						if (PlayerController != TestPlayerController) {
							if (AFPSPlayerState* ps = Cast<AFPSPlayerState>(PlayerController->PlayerState))
							{
								if (NumberOfTeams > 1) {
									ABaseTeam* SmallestTeam = Cast<ABaseTeam>(Teams[0]);
									bool SecondCheck = false;
									//for (int32 loopindex = 0; loopindex < 2; ++loopindex) {
									for (int32 TeamsIndex = 1; TeamsIndex < Teams.Num(); ++TeamsIndex)
									{
										if (ABaseTeam* ATeam = Cast<ABaseTeam>(Teams[TeamsIndex]))
										{
											if (ATeam->TeamPlayerStates.Num() < SmallestTeam->TeamPlayerStates.Num())
											{
												SmallestTeam = ATeam;
											}

										}
									}

									if (ps->Team == NULL)
									{
										ps->SetTeam(SmallestTeam);
										SmallestTeam->AddToPlayerStates(ps);
									
										UE_LOG(LogClass, Log, TEXT("Assigned to team"));

									}
								}
								else if (NumberOfTeams == 1)
								{
									if (ABaseTeam* ATeam = Cast<ABaseTeam>(Teams[0]))
									{
										ps->SetTeam(ATeam);
										ATeam->AddToPlayerStates(ps);
										
									}
								}





							}
						}

					}


				}
					
				int32 PlayersOnTeams = 0;
				for (int32 i = 0; i < Teams.Num(); ++i)
				{
					if (ABaseTeam* team = Cast<ABaseTeam>(Teams[i]))
					{
						PlayersOnTeams += team->TeamPlayerStates.Num();
					}
				}
				if (PlayersOnTeams == GetNumPlayers())
				{
					UE_LOG(LogClass, Log, TEXT("Everyone is on a team"));
					if (AFPSGameState* TheGameState = Cast<AFPSGameState>(GameState))
					{
						//bool TeamsFilled = true;
						for (int32 i = 0; i < Teams.Num(); ++i)
						{
							if (Teams[i]->TeamPlayerStates.Num() <= 0)
							{
								Teams.RemoveAt(i);
							}
						}
						TheGameState->UpdateTeamList(Teams);
					}
					for (int32 team = 0; team < Teams.Num(); ++team)
					{
						if (ABaseTeam* teamclass = Cast<ABaseTeam>(Teams[team]))
						{
							for (int32 playerindex = 0; playerindex < teamclass->TeamPlayerStates.Num(); ++playerindex)
							{
								if (AFPSPlayerState* playerstate = Cast<AFPSPlayerState>(teamclass->TeamPlayerStates[playerindex]))
								{
									if (playerstate->UserName == "000")
									{
										FString newName = "Team" + FString::FromInt(teamclass->TeamNumber) + "Player[" + FString::FromInt(playerindex) + "]";						
										playerstate->SetUserNameMultiCast(FName(*newName));
									}
									if (playerstate->PlayerTag == "000")
									{
										FString newTag = "T0" + FString::FromInt(playerindex);
										playerstate->SetPlayerTagMultiCast(FName(*newTag));
									}
									UE_LOG(LogClass, Log, TEXT("SetPlayerName"));
								}
							}
						}
					}



				

				
					
					NewPlayer->GetPawn()->Destroy();
					int32 StartSpotIndex = FMath::RandRange(0, PreferredStarts.Num() - 1);
					NewPlayer->SetPawn(SpawnDefaultPawnFor(NewPlayer, PreferredStarts[StartSpotIndex]));
					for (TActorIterator<AFPSPlayerStart> ClearPlayerStart(GetWorld()); ClearPlayerStart; ++ClearPlayerStart) {
						ClearPlayerStart->PlayerStartTag = FName(TEXT("Open"));
					}

					PreferredStarts[StartSpotIndex]->PlayerStartTag = FName(TEXT("Blocked"));

					RestartPlayer(NewPlayer);
					AFPSCharacter* Character = Cast<AFPSCharacter>(NewPlayer->GetPawn());
					if (Character)
					{
						if (AFPSPlayerState* charPS = Cast<AFPSPlayerState>(Character->PlayerState))
						{
							charPS->SetMyCharacter(Character);
						}
						Character->TriggerAddUI();
						Character->AddTeamColor();

					}
					for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
					{

						AFPSPlayerController* PC = Cast<AFPSPlayerController>(*It);
						PC->AddScoreBoardUI();
						PC->TriggerAddAliveUI();
						if (PC != NewPlayer)
						{
							PC->ServerSetPlayerTeamClient(0);
						}
					}
					HandleNewState(EGamePlayState::EPlaying);
					

				}
			
			}
		}
	}

	for (int32 i = 0; i < WeaponSpawns.Num(); ++i)
	{
		if (WeaponSpawns[i]->SpawnOnStart == true)
		{
			WeaponSpawns[i]->SpawnWeapon();
		}
	}
	for (int32 i = 0; i < GrenadeSpawns.Num(); ++i)
	{
		if (GrenadeSpawns[i]->SpawnOnStart == true)
		{
			GrenadeSpawns[i]->SpawnGrenade();
		}
	}

	//TArray<AFPSPlayerController> PlayerControllers;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFPSPlayerController::StaticClass(), PlayerControllers);


}







void AFPSProjectGameModeBase::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);

}


void AFPSProjectGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	check(World);
	AFPSGameState* MyGameState = Cast<AFPSGameState>(GameState);
	check(MyGameState);
	MyGameState->ClientUpdateNumberOfTeams(NumberOfTeams);

	//HandleNewState(EGamePlayState::EPlaying);

	for (int32 i = 0; i < NumberOfTeams; ++i)
	{
		ABaseTeam* Team = GetWorld()->SpawnActor<ABaseTeam>(BaseTeamObj, FVector(0, 0, 20), FRotator(0, 0, 0));
		Team->TeamNumber = i + 1;
		if (i == 0)
		{
			Team->TeamName = FName(TEXT("Blue Team"));
			Team->TeamColor = FColor::Blue;
		}
		if (i == 1)
		{
			Team->TeamName = FName(TEXT("Red Team"));
			Team->TeamColor = FColor::Red;
		}
		if (i == 2)
		{
			Team->TeamName = FName(TEXT("Green Team"));
			Team->TeamColor = FColor::Green;
		}
		if (i == 3)
		{
			Team->TeamName = FName(TEXT("Yellow Team"));
			Team->TeamColor = FColor::Yellow;
		}
		Teams.Add(Team);
		if (Team)
		{
			UE_LOG(LogClass, Log, TEXT("Number of Teams: %d"), Teams.Num());
		}
	}

	MyGameState->SetNumberOfPlayers(GetNumPlayers());
	
	for (TActorIterator<AWeaponSpawner> WeaponSpawner(GetWorld()); WeaponSpawner; ++WeaponSpawner)
	{
		WeaponSpawns.Add(*WeaponSpawner);
	}
	for (TActorIterator<AGrenadeSpawner> GrenadeSpawner(GetWorld()); GrenadeSpawner; ++GrenadeSpawner)
	{
		GrenadeSpawns.Add(*GrenadeSpawner);
	}

	

}

void AFPSProjectGameModeBase::KillPlayer(AFPSCharacter * Killed, AFPSCharacter * Killer)
{
	if (Killed->GetCurrentHealth() <= 0.0f)
	{
		UE_LOG(LogClass, Log, TEXT("%s should be killed by %s"), *Killed->GetName(), *Killer->GetName());
		if (AFPSPlayerState* killedPS = Cast<AFPSPlayerState>(Killed->PlayerState))
		{
			if (AFPSPlayerState* killerPS = Cast<AFPSPlayerState>(Killer->PlayerState))
			{
				if (AFPSGameState* gameState = Cast<AFPSGameState>(GameState))
				{
					killedPS->SetDeathsMultiCast(1);
					if (Killed != Killer)
					{
						if (killedPS->Team != killerPS->Team)
						{
							killerPS->SetKillsMultiCast(1);
							killerPS->SetScoreMultiCast(10);
							gameState->LastKiller = killerPS->UserName;
							gameState->LastKilled = killedPS->UserName;


							gameState->KillFeedMessage = TEXT("" + gameState->LastKiller + " killed " + gameState->LastKilled);

							gameState->ClientUpdateKillFeedMessage(FName(*gameState->KillFeedMessage));
							killerPS->Team->ChangeScore(PointsPerKill);
						}
						else
						{
							gameState->LastKiller = killerPS->UserName;
							gameState->LastKilled = killedPS->UserName;

							gameState->KillFeedMessage = TEXT("" + gameState->LastKiller + " betrayed " + gameState->LastKilled);
							killerPS->Betrayals += 1;
							gameState->ClientUpdateKillFeedMessage(FName(*gameState->KillFeedMessage));
						}


					}
				}
			}
			UpdateStats();
			if (AFPSGameState* gameState = Cast<AFPSGameState>(GameState))
			{
				gameState->CallHUDUpdate();
			}
			Killed->OnDeathRemoveUI();
			Killed->IsDead = true;
			//Player->DropWeapon();
			//Player->DropEquipment();
			Killed->OnPlayerDeath();
		}
	}
}

void AFPSProjectGameModeBase::UpdateStats()
{
	////////////////////////////////////////////////////////////////////////////////////////////////
	for (int32 i = 0; i < Teams.Num(); ++i)
	{
		if (ABaseTeam* Team = Cast<ABaseTeam>(Teams[i]))
		{
			int32 BestScore = -1;
			TArray<AFPSPlayerState*> CorrectTeamOrder;
			for (int32 psindex = 0; psindex < Team->TeamPlayerStates.Num(); ++psindex)
			{
				if (AFPSPlayerState* playerstate = Cast<AFPSPlayerState>(Team->TeamPlayerStates[psindex]))
				{
					if (playerstate->GetScore() > BestScore)
					{
						CorrectTeamOrder.Insert(playerstate, 0);
						BestScore = playerstate->GetScore();
					}
					else if (CorrectTeamOrder.Num() > 1)
					{
						for (int32 otherpsindex = 0; otherpsindex < CorrectTeamOrder.Num(); ++otherpsindex)
						{
							if (AFPSPlayerState* otherps = Cast<AFPSPlayerState>(CorrectTeamOrder[otherpsindex]))
							{
								if (CorrectTeamOrder.Contains(playerstate) == false)
								{
									if (playerstate->GetScore() > otherps->GetScore())
									{
										CorrectTeamOrder.Insert(playerstate, otherpsindex);
									}
								}
							}
							else
							{
								CorrectTeamOrder.Emplace(playerstate);
							}
						}
						if (CorrectTeamOrder.Contains(playerstate) == false)
						{
							CorrectTeamOrder.Emplace(playerstate);
						}
					}
					else
					{
						if (CorrectTeamOrder.Contains(playerstate) == false)
						{
							CorrectTeamOrder.Emplace(playerstate);
						}
					}
				}
			}
			Team->UpdateTeamOrder(CorrectTeamOrder);
		}
	}

	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It) {
		if (APlayerController* PlayerController = Cast<APlayerController>(*It)) {
			if (AFPSPlayerController* pc = Cast<AFPSPlayerController>(PlayerController))
			{
				pc->UpdateScoreBoardUI();
			}

		}


		if (AFPSPlayerController* FPSController = Cast<AFPSPlayerController>(*It))
		{



			if (AFPSPlayerState* ps = Cast<AFPSPlayerState>(FPSController->PlayerState))
			{


				float ScorePercent = ((double)ps->Team->TeamScore / ScoreToWin);

				FPSController->UpdateMyTeamStats(ps->Team->TeamScore, ScorePercent);



			}


			AFPSPlayerController* MyRival = NULL;
			int32 RivalDifference = 100000;
			for (FConstControllerIterator OtherControllers = GetWorld()->GetControllerIterator(); OtherControllers; ++OtherControllers) {
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
					


					float RivalScorePercent = ((double)Rivalps->Team->TeamScore / ScoreToWin);

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




			if (AFPSGameState* gameState = Cast<AFPSGameState>(GameState))
			{
				for (int32 i = 0; i < Teams.Num(); ++i)
				{
					if (ABaseTeam* team = Cast<ABaseTeam>(Teams[i]))
					{
						if (team->TeamScore >= ScoreToWin)
						{
							gameState->WinningTeam = team->TeamNumber;
							gameState->WinningMessage = team->TeamName.ToString() + " Won";
							gameState->ClientUpdateWinningTeam(gameState->WinningTeam, FName(*gameState->WinningMessage));
							gameState->CallHUDGameOver();
							for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It) {
								if (AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(*It)) {
									if (AFPSCharacter* Player = Cast<AFPSCharacter>(PlayerController->GetPawn())) {
										Player->SetCurrentState(EPlayerState::EPlayerWaiting);
									}

								}

							}
							gameState->SetCurrentState(EGamePlayState::EGameOver);
						}

					}
				}
			}
			
		}
	}
}

void AFPSProjectGameModeBase::AllowPlay()
{
	AFPSGameState* MyGameState = Cast<AFPSGameState>(GameState);
	check(MyGameState);
	MyGameState->SetCurrentState(EGamePlayState::EPlaying);
	for (TActorIterator<AFPSCharacter> Character(GetWorld()); Character; ++Character)
	{
		Character->SetCurrentState(EPlayerState::EPlayerPlaying);
	}
	UE_LOG(LogClass, Log, TEXT("Allow Play Now"));
	GameCountdownTimer.Invalidate();
}


void AFPSProjectGameModeBase::HandleNewState(enum EGamePlayState NewState)
{
	UWorld* World = GetWorld();
	check(World);
	AFPSGameState* MyGameState = Cast<AFPSGameState>(GameState);
	check(MyGameState);

	if (NewState != MyGameState->GetCurrentState())
	{
		//update the state, so clients know about the transition
		

		switch (NewState)
		{
		case EGamePlayState::EWaiting:

			break;
		case EGamePlayState::EPlaying:
			GetWorld()->GetTimerManager().SetTimer(GameCountdownTimer, this, &AFPSProjectGameModeBase::AllowPlay, GameCountdownDelay, false);
			
			for (TActorIterator<AFPSCharacter> Character(GetWorld()); Character; ++Character)
			{
				Character->SetCurrentState(EPlayerState::EPlayerWaiting);
			}
			break;
		case EGamePlayState::EGameOver:
			MyGameState->SetCurrentState(NewState);

			break;
		default:
		case EGamePlayState::EUnknown:
			break;
		}
	}

}



