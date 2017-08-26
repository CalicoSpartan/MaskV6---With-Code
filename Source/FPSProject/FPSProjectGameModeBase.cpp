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

void AFPSProjectGameModeBase::RespawnPlayer(APlayerController* NewPlayer)
{
	UE_LOG(LogClass, Log, TEXT("Respawning"));
	AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(NewPlayer);
	TArray<AFPSPlayerStart*> PreferredStarts;
	bool checkforsafety = true;
	for (int32 i = 0; i < 5; ++i) {
		for (TActorIterator<AFPSPlayerStart> PlayerStart(GetWorld()); PlayerStart; ++PlayerStart)
		{
			if (PlayerController)
			{
				if (PlayerStart->Tags.Contains("RespawnPoint"))
				{
					for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
					{
						AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(*Iterator);
						if (PlayerController)
						{
							if (PlayerController != NewPlayer) {
								if (AFPSPlayerController* NewFPSPlayer = Cast<AFPSPlayerController>(NewPlayer))
								{

									if (AFPSPlayerState* playerstate = Cast<AFPSPlayerState>(PlayerController->PlayerState)) {
										if (AFPSPlayerState* fpsplayerstate = Cast<AFPSPlayerState>(NewFPSPlayer->PlayerState)) {
											if (playerstate->Team != fpsplayerstate->Team)
											{
												AFPSCharacter* PlayerCharacter = Cast<AFPSCharacter>(PlayerController->GetPawn());
												if (PlayerCharacter) {
													UCameraComponent* PlayerCamera = Cast<UCameraComponent>(PlayerCharacter->FPSCameraComponent);
													if (PlayerCamera)
													{
														FVector CameraLocation = PlayerCamera->GetComponentLocation();
														FVector SpawnPointLocation = PlayerStart->GetActorLocation();
														FVector DirectionBetween = (SpawnPointLocation - CameraLocation).GetSafeNormal();
														FVector CameraDirection = PlayerCamera->GetForwardVector().GetSafeNormal();
														float dotvalue = FGenericPlatformMath::Abs(FVector::DotProduct(DirectionBetween, CameraDirection));

														if (dotvalue > .68f) {


															FHitResult hit;

															if (GetWorld()->LineTraceSingleByChannel(hit, CameraLocation, CameraLocation + (DirectionBetween * 10000), ECollisionChannel::ECC_Camera))
															{


																if (AFPSPlayerStart* spawnpoint = Cast<AFPSPlayerStart>(hit.GetActor())) {

																}
																else {


																	PreferredStarts.AddUnique(*PlayerStart);
																}
															}

															if (i >= 3) {
																UE_LOG(LogClass, Log, TEXT("Respawning anyway"));
																PreferredStarts.AddUnique(*PlayerStart);
															}

														}
														else
														{
															if (FVector::Dist(CameraLocation, SpawnPointLocation) > SafeSpawnDistance || i >= 3) {
																PreferredStarts.AddUnique(*PlayerStart);
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
		}
		if (PreferredStarts.Num() == 0) {

		}
		else
		{
			break;
		}
	}
	if (PreferredStarts.Num() > 1) {
		AFPSPlayerStart* BestStart = PreferredStarts[FMath::RandRange(0, PreferredStarts.Num() - 1)];
		float SpawnDistance = 0.0f;
		for (int32 i = 0; i < PreferredStarts.Num(); ++i) {
			for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
			{

				AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(*Iterator);
				if (PlayerController)
				{
					if (PlayerController != NewPlayer) {
						if (AFPSPlayerController* NewFPSPlayer = Cast<AFPSPlayerController>(NewPlayer))
						{
							if (AFPSPlayerState* playerstate = Cast<AFPSPlayerState>(PlayerController->PlayerState)) {
								if (AFPSPlayerState* fpsplayerstate = Cast<AFPSPlayerState>(NewFPSPlayer->PlayerState)) {
									if (playerstate->Team != fpsplayerstate->Team)
									{


										AFPSCharacter* PlayerCharacter = Cast<AFPSCharacter>(PlayerController->GetPawn());
										if (PlayerCharacter) {
											UCameraComponent* PlayerCamera = Cast<UCameraComponent>(PlayerCharacter->FPSCameraComponent);
											if (PlayerCamera)
											{
												FVector CameraLocation = PlayerCamera->GetComponentLocation();
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
						/*
						else if (NewPlayerState->Team->TeamNumber == 2 && PlayerStart->Tags.Contains("Team2")) //&& PlayerStart->PlayerStartTag != FName(TEXT("Blocked")))
						{
							if (PlayerStart->PlayerStartTag == FName(TEXT("Blocked"))) {
								if (BlockCheck == false) {

									PreferredStarts.AddUnique(*PlayerStart);
								}
								else {

								}

							}
							else {

								PreferredStarts.AddUnique(*PlayerStart);
							}
							// Player should spawn on Suspects


						}
						*/
						

					}

				}
				if (PreferredStarts.Num() == 0) {
					BlockCheck = false;
				}
				else {
					break;
				}

			}
			/*
			if (NewPlayerState->Team->TeamNumber == 0)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Player is a spectator");
				return;
			}
			else
			{
			*/
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
						//TestPlayerController->TriggerAddAliveUI();
					}
					//TestPlayerController->TriggerAddAliveUI();
				}
				UE_LOG(LogClass, Log, TEXT("client controller has team and spawned in"));
			}
			//}

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
					Team1->TeamPlayerStates.Add(MyPlayerState);
					MyPlayerState->SetTeam(Team1);
					
				}
			}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			
			TArray<AFPSPlayerStart*> PreferredStarts;
			if (AFPSGameState* MyGameState = Cast<AFPSGameState>(GameState))
			{
				MyGameState->SetNumberOfPlayers(GetNumPlayers());
			}


			for (TActorIterator<AFPSPlayerStart> PlayerStart(GetWorld()); PlayerStart; ++PlayerStart)
			{




				if (MyPlayerState->Team->TeamNumber == PlayerStart->TeamNumber)
				{

					// Player should spawn on CT.
					PreferredStarts.Add(*PlayerStart);

				}

			
			}
			/*
			if (MyPlayerState->Team->TeamNumber == 0)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Player is a spectator");
				return;
			}
			else
			{
				
				if (TestPlayerController->GetPlayerTeam() == 1) {


					if (AFPSGameState* MyGameState = Cast<AFPSGameState>(GameState))
					{
						MyGameState->ClientUpdateTeam1Controllers(TestPlayerController);
						if (AFPSPlayerState* ps = Cast<AFPSPlayerState>(TestPlayerController->PlayerState))
						{
							MyGameState->ClientUpdateTeam1PlayerStates(ps);
						}

					}

					UE_LOG(LogClass, Log, TEXT("Team1PlayerControllerslength: %d"), Team1PlayerControllers.Num());
					UE_LOG(LogClass, Log, TEXT("Team2PlayerControllerslength: %d"), Team2PlayerControllers.Num());

					{
						//MyGameState->ClientUpdateTeam1Controllers(TestPlayerController);
						//UE_LOG(LogClass, Log, TEXT("added server to team 1"));

					}
				}
				if (TestPlayerController->GetPlayerTeam() == 2) {
					if (AFPSGameState* MyGameState = Cast<AFPSGameState>(GameState))
					{
						MyGameState->ClientUpdateTeam2Controllers(TestPlayerController);
						if (AFPSPlayerState* ps = Cast<AFPSPlayerState>(TestPlayerController->PlayerState))
						{
							MyGameState->ClientUpdateTeam2PlayerStates(ps);
						}
					}

					UE_LOG(LogClass, Log, TEXT("Team1PlayerControllerslength: %d"), Team1PlayerControllers.Num());
					UE_LOG(LogClass, Log, TEXT("Team2PlayerControllerslength: %d"), Team2PlayerControllers.Num());

				}
				
			*/


				
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
										SmallestTeam->TeamPlayerStates.AddUnique(ps);
										UE_LOG(LogClass, Log, TEXT("Assigned to team"));

									}
								}
								else if (NumberOfTeams == 1)
								{
									if (ABaseTeam* ATeam = Cast<ABaseTeam>(Teams[0]))
									{
										ps->SetTeam(ATeam);
										ATeam->TeamPlayerStates.Add(ps);
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
									FString newName = "Team" + FString::FromInt(teamclass->TeamNumber) + "Player[" + FString::FromInt(playerindex) + "]";
									playerstate->SetUserNameMultiCast(FName(*newName));
									UE_LOG(LogClass, Log, TEXT("SetPlayerName"));
								}
							}
						}
					}



				

				
					HandleNewState(EGamePlayState::EPlaying);
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
					

				}
			
			}
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
			Team->TeamColor = FColor::Blue;
		}
		if (i == 1)
		{
			Team->TeamColor = FColor::Red;
		}
		if (i == 2)
		{
			Team->TeamColor = FColor::Green;
		}
		if (i == 3)
		{
			Team->TeamColor = FColor::Yellow;
		}
		Teams.Add(Team);
		if (Team)
		{
			UE_LOG(LogClass, Log, TEXT("Number of Teams: %d"), Teams.Num());
		}
	}

	MyGameState->SetNumberOfPlayers(GetNumPlayers());



}



void AFPSProjectGameModeBase::HandleNewState(EGamePlayState NewState)
{
	UWorld* World = GetWorld();
	check(World);
	AFPSGameState* MyGameState = Cast<AFPSGameState>(GameState);
	check(MyGameState);

	if (NewState != MyGameState->GetCurrentState())
	{
		//update the state, so clients know about the transition
		MyGameState->SetCurrentState(NewState);

		switch (NewState)
		{
		case EGamePlayState::EWaiting:

			break;
		case EGamePlayState::EPlaying:


			break;
		case EGamePlayState::EGameOver:


			break;
		default:
		case EGamePlayState::EUnknown:
			break;
		}
	}

}


