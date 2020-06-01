// Fill out your copyright notice in the Description page of Project Settings.

#include "InvadedGM.h"
#include "Kismet/GameplayStatics.h"
#include "GridGenerator.h"
#include "InvadedPC.h"
#include "InvadedPlayer.h"
#include "InvadedGS.h"
#include "InvadedData.h"
#include "Dice.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "Engine.h"
#include "Flag.h"
#include "InvadedPlayerStart.h"
#include "DicePod.h"
#include "Actors/Cards/CardBase.h"
#include "Interfaces/FightInterface.h"
#include "AI/InvadedAIPlayer.h"
#include "Player/SpectatorPlayer.h"
#include "CellBase.h"
#include "Player/InvadedMovementComp.h"
#include "InvadedPS.h"
#include "Engine/World.h"
#include "LobbySG.h"
#include "MiniGames/WordGame.h"
#include "Engine/ActorChannel.h"
#include "Online.h"
#include "OnlineLeaderboardInterface.h"

void AInvadedGM::BeginPlay()
{
	Super::BeginPlay();
	

	FActorSpawnParameters SpawnParams;
	TArray<AActor*> Locations;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("GridLocation"), Locations);
	FVector Location;
	FRotator Rotation;
	if (Locations.Num())
	{
		Location = Locations[0]->GetActorLocation();
		Rotation = Locations[0]->GetActorRotation();
	}

	GridGenerator = GetWorld()->SpawnActor<AGridGenerator>(GridGeneratorClass, Location, Rotation, SpawnParams);
	if (GridGenerator)
	{
	//	GridGenerator->SetMapScale(MapScale);

		UE_LOG(LogTemp, Warning, TEXT("Map Scale %f"), InvadedGS->GetMapInfo().MapScale);

		GridGenerator->SetMapScale(InvadedGS->GetMapInfo().MapScale);

		GridGenerator->SetMapInfo(InvadedGS->GetMapInfo());

		GridGenerator->Generate();
	}

	if (InvadedGS)
	{
		InvadedGS->SetGridGenerator(GridGenerator);
	}

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ADicePod::StaticClass(), Actors); 
	if (Actors.Num())
	{
		DicePod = Cast<ADicePod>(Actors[0]);
	}
	Actors.Empty();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInvadedAIPlayer::StaticClass(), Actors);
	int32 i = 1;
	for (AActor* Actor : Actors)
	{
		AInvadedAIPlayer* AIPlayer = Cast<AInvadedAIPlayer>(Actor);
		if (AIPlayer)
		{
			i++;
		}
		
	}

	bPlayersReady = false;

	double secs = FTimespan(FDateTime::Now().GetTicks()).GetTotalSeconds();
	int32 seed = (int32)(((int64)secs) % INT_MAX);
	FMath::RandInit(seed);

	
}
TSubclassOf<class ACardBase> AInvadedGM::DrawAttackCard()
{
	uint32 Index = FMath::RandRange(0, AttackCardsClasses.Num() - 1);
	return AttackCardsClasses[Index];
}
TSubclassOf<class ACardBase> AInvadedGM::DrawDefensiveCard()
{
	uint32 Index = FMath::RandRange(0, DefensiveCardsClasses.Num() - 1);
	return DefensiveCardsClasses[Index];
}

void AInvadedGM::SetupUI()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInvadedPlayer::StaticClass(), Actors);
	for (AActor* Actor : Actors)
	{
		AInvadedPlayer* InvadedPlayer = Cast<AInvadedPlayer>(Actor);
		if (InvadedPlayer)
		{
			if (!InvadedPlayer->GetStartAsSpectator())
			{
				
				InvadedPlayer->AddGameUI();
				if (bStartWithFight)
				{
					InvadedPlayer->AddGameStartUI();
				}
				else
				{
					InvadedPlayer->PushState("GameUI");
				}

				
			}
			
		}
		
	}
	
}
void AInvadedGM::SetMapScale(float Value)
{
	MapScale = Value;
}
void AInvadedGM::InitGameState()
{
	Super::InitGameState();
	InvadedGS = Cast<AInvadedGS>(GameState);
	if (InvadedGS&&!bIsInEditor)
	{
		InvadedGS->LoadVariables(0);

		MaxNumOfPlayers = 4;
		if (InvadedAIPlayerClass)
		{
			for (int i = 0; i < MaxNumOfPlayers - InvadedGS->GetNumberOfPlayers(); ++i)
			{
				FActorSpawnParameters SpawnParams;
				FTransform Transform;
				//Transform.SetScale3D(FVector(1.0f,1.0f,1.0f));
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
				AInvadedAIPlayer* AIPlayer = GetWorld()->SpawnActor<AInvadedAIPlayer>(InvadedAIPlayerClass, Transform, SpawnParams);
				if (AIPlayer)
				{
					AIPlayer->SetupPlayer(GenerateActorName(), i + 1);

					InvadedGS->AddInvadedPlayerState(AIPlayer->GetInvadedPS());
					AIPlayers.Add(AIPlayer);

					AIPlayer->GetInvadedPS()->SetIsAIPlayer(true);
					PlayerStates.Add(AIPlayer->GetInvadedPS());

					Players.Add(AIPlayer);
				}
				
			}
		}
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInvadedAIPlayer::StaticClass(), Actors);
		InvadedGS->SetCurrentNumOfPlayers(MaxNumOfPlayers - InvadedGS->GetNumberOfPlayers());
		InvadedGS->SetNumberOfPlayers(InvadedGS->GetNumberOfPlayers() + Actors.Num());
		InvadedGS->SetNumberOfAlivePlayers(InvadedGS->GetNumberOfPlayers());
		
	}
	else if (InvadedGS)
	{
		
		if (InvadedAIPlayerClass)
		{
			for (int i = 0; i < MaxNumOfPlayers - int32(NumOfPlayers); ++i)
			{
				UE_LOG(LogTemp, Warning, TEXT("SpawningPlayers"));
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
				FTransform Transform;
				AInvadedAIPlayer* AIPlayer=GetWorld()->SpawnActor<AInvadedAIPlayer>(InvadedAIPlayerClass, Transform, SpawnParams);
				if (AIPlayer)
				{
					AIPlayer->SetupPlayer(GenerateActorName(), i + 1);

					AIPlayers.Add(AIPlayer);

					PlayerStates.Add(AIPlayer->GetInvadedPS());

					InvadedGS->AddInvadedPlayerState(AIPlayer->GetInvadedPS());

					Players.Add(AIPlayer);
				}
				
		
			}
		}
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInvadedAIPlayer::StaticClass(), Actors);
		InvadedGS->SetNumberOfPlayers(NumOfPlayers+Actors.Num());
		InvadedGS->SetNumberOfAlivePlayers(NumOfPlayers + Actors.Num());
		InvadedGS->SetCurrentNumOfPlayers(Actors.Num());
	}
	if (InvadedGS)
	{
		InvadedGS->SetMapScale(MapScale);
		InvadedGS->SetMaxTurnTime(MaxTurnTime);
	}
		
	UE_LOG(LogTemp, Warning, TEXT("Map Scale %f"), InvadedGS->GetMapInfo().MapScale);
	
}

void AInvadedGM::ThrowDice()
{
	UE_LOG(LogTemp, Warning, TEXT("Dice Throwing"));
	if (DicePod)
	{	

		DicePod->SpawnDice(bAdditionalThrow);
		bAdditionalThrow = false;

	}
	GetWorldTimerManager().ClearTimer(ThrowDiceHandle);
	
}
void AInvadedGM::OnEndTurn()
{
	if (InvadedGS&&!GameEnded)
	{
		bAdditionalThrow = false;
		if (InvadedGS->GetCurrentPlayerIndex() >= InvadedGS->GetNumberOfPlayers())
		{
			
		
			NextTurn();
		}
		else
		{
			SetNextCurrentPlayer(0);
		}
	}
}
void AInvadedGM::NextTurn()
{
	UE_LOG(LogTemp, Warning, TEXT("NEXT TURN"));
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInvadedCharacter::StaticClass(), Actors);
	if (InvadedGS)
	{
		InvadedGS->SetCurrentPlayerIndex(0);
		
		InvadedGS->SetCurrentTurn(InvadedGS->GetCurrentTurn() + 1);
		SetNextCurrentPlayer(0);

	}
}
void AInvadedGM::DiceHaveLanded(AActor * Dice, uint8 Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Dice has landed %d"),Value);
	if (DicePod)
	{
		DicePod->Toggle();
	}
}

void AInvadedGM::SetNextCurrentPlayer(const uint8& StartingIndex)
{
	if (InvadedGS)
	{
		UE_LOG(LogTemp, Warning, TEXT("SET NEXT CURRENT PLAYER %d" ), InvadedGS->GetCurrentPlayerIndex() + StartingIndex);
		uint8 Index = InvadedGS->GetCurrentPlayerIndex()+StartingIndex;
		
		if (InvadedGS->GetNumberOfPlayers()> Index)
		{
			 UInvadedData* InvadedPS = PlayerStates[Index];
			if (InvadedPS)
			{
			
				if (SkipTurnPlayers.Find(InvadedPS) != INDEX_NONE)
				{
					SkipTurnPlayers.Remove(InvadedPS);
					SetNextCurrentPlayer(StartingIndex+1);
				}
				else if (InvadedPS->GetIsAlive() == false||InvadedPS->GetOwner()==nullptr)
				{
					SetNextCurrentPlayer(StartingIndex + 1);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("NEXT PLAYER"));
						
					InvadedGS->SetCurrentPlayer(InvadedPS);
					InvadedPS->SetIsCurrentPlayer(true);
					InvadedPS->SetHasTrownDice(false);
				
					DicePod->SetFirstColor(InvadedPS->GetFlagColor());
					DicePod->SetFirstColor(InvadedPS->GetPlayerColor());
					ToggleDicePod();

					if (InvadedPS->GetOwner()->IsA<AInvadedAIPlayer>())
					{
						for (AInvadedAIPlayer* AIPlayer : AIPlayers)
						{
							AIPlayer->NextTurn(InvadedPS);
						}
					}					
				}
	
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Player State Null Index %d"), Index);
			}
		}
		else
		{
				NextTurn();	
		}
	}	
}
void AInvadedGM::ChangePlayerCamera()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInvadedPlayer::StaticClass(), Actors);
	for (AActor* Actor : Actors)
	{
		AInvadedPlayer* InvadedPlayer = Cast<AInvadedPlayer>(Actor);
		if (InvadedPlayer)
		{
			
			AInvadedPC* InvadedPC = Cast<AInvadedPC>(InvadedPlayer->GetController());
			if (InvadedPC)
			{

				InvadedPC->LookAtPawn();
			}

		}

	}
	SetupUI();
}
void AInvadedGM::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	AInvadedPC* InvadedPC = Cast<AInvadedPC>(NewPlayer);
	if (InvadedPC)
	{
		//InvadedPC->LookAtScreen();
	}
}
void AInvadedGM::ToggleDicePod()
{
	if (!GameEnded)
	{
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInvadedCharacter::StaticClass(), Actors);

		

		if (Actors.Num())
		{
			for (AActor* Actor : Actors)
			{
				AInvadedCharacter* Player = Cast<AInvadedCharacter>(Actor);
				if (Player)
				{
					Player->ToggleDicePod();
				}
			}
		}
		else
		{
			for (AInvadedCharacter* Player : Players)
			{
				if (Player != nullptr)
				{
					Player->ToggleDicePod();
				}
			}
		}
		
		Actors.Empty();
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpectatorPlayer::StaticClass(), Actors);
		for (AActor* Actor : Actors)
		{
			ASpectatorPlayer* Player = Cast<ASpectatorPlayer>(Actor);
			if (Player)
			{
				Player->ToggleDicePod();
			}
		}

		

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GameEnded"));
	}
	
	
}
void AInvadedGM::ConfigurateFlags(TArray<AFlag*> FlagArray)
{
	TArray<FName> PlayerNames;
	TArray<FLinearColor> PlayerFirstColors;
	TArray<FLinearColor> PlayerSecondColors;
	TArray<int32> FlagTextureIndexes;

	for (UInvadedData* InvadedPS : PlayerStates)
	{
		if (InvadedPS)
		{
		
			PlayerNames.Add(FName(*InvadedPS->GetPlayerName()));
			PlayerFirstColors.Add(InvadedPS->GetFlagColor());
			PlayerSecondColors.Add(InvadedPS->GetPlayerColor());
			FlagTextureIndexes.Add(InvadedPS->GetFlagTextureIndex());

			if(InvadedPS->GetIsAIPlayer()==false)
			UE_LOG(LogTemp, Warning, TEXT("Flag Color %s"), *InvadedPS->GetPlayerColor().ToString());
		
		}
	}
	for (int32 i = 0; i < FlagArray.Num(); ++i)
	{
		if (Players.Num()>i)
		{
			FlagArray[i]->SetOwnerName(PlayerNames[i]);
			FlagArray[i]->SetFirstOwner(Cast<AInvadedCharacter>(Players[i]));
			FlagArray[i]->SetFirstColor(PlayerFirstColors[i]);
			FlagArray[i]->SetSecondColor(PlayerSecondColors[i]);
			FlagArray[i]->SetFlagTextureIndex(FlagTextureIndexes[i]);
	
			FlagArray[i]->FinishSetup();
		}
		

	}
	Flags = FlagArray;
	if (bStartWithFlag)
		AddFlagsToPlayers(Flags);
}
void AInvadedGM::DestroyFlag(class AFlag* Flag, class AInvadedCharacter* Player, AInvadedCharacter* Causer)
{

	if (!Flag)
		UE_LOG(LogTemp, Warning, TEXT("Flag nullptr"));
	if (!Player)
		UE_LOG(LogTemp, Warning, TEXT("Player nullptr"));
	if (Flag&&Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Destroy Flag %s %d"), *Player->GetInvadedPS()->GetPlayerName(), InvadedGS->GetNumberOfPlayers());

		UInvadedData* InvadedPS = Player->GetInvadedPS();
		if (InvadedPS)
		{

			Player->GetOversteppedCell()->SetIsOccupied(false);
			Player->GetOversteppedCell()->SetOccupyingPlayer(nullptr);

			InvadedPS->SetTurnScore(InvadedGS->GetCurrentTurn());
			Causer->GetInvadedPS()->SetTurnScore(InvadedGS->GetCurrentTurn());
			UpdateLeaderboardPlayerData(InvadedPS);
			InvadedPS->SetIsAlive(false);
			PlayerStates.Remove(InvadedPS);

			InvadedGS->SetNumberOfPlayers(InvadedGS->GetNumberOfPlayers() - 1);

			Causer->KilledPlayer();

			

			Player->RemoveFlag(Flag);

			Flag->Destroy();

			Player->GameLost();



			GridGenerator->DisableQuadrant(Flag->GetDeathCell(), Causer);


			if (InvadedGS->GetNumberOfPlayers() == 1)
			{
				AsyncTask(ENamedThreads::GameThread, [=]()
				{
					GetWorld()->GetTimerManager().SetTimer(EndGameHandle, this, &AInvadedGM::EndGame, 1.5f, false);
					if (Causer&&Causer->GetInvadedPS())
					{
						Causer->GetInvadedPS()->SetRemainingMoves(0);
						Causer->GetInvadedPS()->SetIsCurrentPlayer(false);
					}
					//EndGame();
				});
			}

			//Causer->NextPathCell();
		}

	}
}
void AInvadedGM::InitPlayerLocation()
{
	if(!bPlayersReady)
	GetWorldTimerManager().SetTimer(PlayerLocationHandle, this, &AInvadedGM::SetPlayerStart, InitPlayerLocationDelay, false);
}
void AInvadedGM::SetPlayerStart()
{	
	bPlayersReady = true;
	FTimerHandle ChangeCameraHandle;
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInvadedPlayer::StaticClass(), Actors);
	for (AActor* Actor : Actors)
	{

		AInvadedPlayer* InvadedPlayer = Cast<AInvadedPlayer>(Actor);
		
		if (InvadedPlayer)
		{
			if (InvadedPlayer->GetStartAsSpectator())
				InvadedPlayer->GameLost();

			InvadedGS->AddInvadedPlayerState(InvadedPlayer->GetInvadedPS());
			PlayerStates.Add(InvadedPlayer->GetInvadedPS());
			Players.Add(InvadedPlayer);
		}
	}
	GridGenerator->SetPlayerStart();
	GridGenerator->SpawnFlags();
	if (bStartWithCards)
	{
		AddCardsToPlayer();
	}
	if (Flags.Num())
	{
		ConfigurateFlags(Flags);
		if (PlayerStates.Num() == InvadedGS->GetNumberOfPlayers())
		{
			if (bStartWithFight)
			{
				SetupFirstFight(InvadedGS->GetPlayerStates());
			}
			else
			{
				NextTurn();
			}
		}
		
	}
	
	GetWorldTimerManager().SetTimer(ChangeCameraHandle,this, &AInvadedGM::ChangePlayerCamera, ChangeCameraDelay, false);

	GetWorldTimerManager().ClearTimer(PlayerLocationHandle);
}
void AInvadedGM::SetupFirstFight(TArray<class UInvadedData*> InvadedDataArray)
{
	
	FirstFightArray = InvadedDataArray;
	FirstFightCounter = 0;
	FirstFight();
}
void AInvadedGM::FirstFight()
{
	
	if (FirstFightArray.Num() > FirstFightCounter)
	{
		UInvadedData* CurrentPlayer = FirstFightArray[FirstFightCounter];
		FirstFightCounter++;
		InvadedGS->SetCurrentPlayer(CurrentPlayer);
		if (CurrentPlayer->GetIsAIPlayer())
		{
			
			ThrowDiceSpecified(CurrentPlayer);
			
		}


	}
	else
	{
		int32 MaxValue=0;
		for (UInvadedData* InvadedData : FirstFightArray)
		{
			if (InvadedData->GetRemainingMoves() > MaxValue)
				MaxValue = InvadedData->GetRemainingMoves();
		}
		UE_LOG(LogTemp, Warning, TEXT("Maxvalue %d"), MaxValue);
		TArray<UInvadedData*> TempArray;
		
		for (UInvadedData* InvadedData : FirstFightArray)
		{
			if (InvadedData->GetRemainingMoves() == MaxValue)
				TempArray.Add(InvadedData);
			else
				OrderArray.Add(TPair<int32, int32>(InvadedGS->GetPlayerStates().Find(InvadedData), InvadedData->GetRemainingMoves()));
		}
		if (TempArray.Num() > 1)
		{
			SetupFirstFight(TempArray);
		}
		else
		{
			OrderArray.Add(TPair<int32, int32>(InvadedGS->GetPlayerStates().Find(TempArray[0]), TempArray[0]->GetRemainingMoves()));

			OrderArray.Sort([](const TPair<int32, int32>& One,const TPair<int32, int32>& Two) {
				return One.Value > Two.Value;
			});

			int32 WinnerIndex = OrderArray[0].Key;

			for (int i = WinnerIndex+1; i < 4; ++i)
			{
				OrderArray[i - WinnerIndex].Key = InvadedGS->GetPlayerStates().Find(InvadedGS->GetPlayerStates()[i]);
			}
			for (int i = 0; i < WinnerIndex; ++i)
			{
				OrderArray[4-WinnerIndex+i].Key = InvadedGS->GetPlayerStates().Find(InvadedGS->GetPlayerStates()[i]);
			}
			TempArray.Empty();
			int32 i = 1;
			for (TPair<int32, int32> Pair: OrderArray)
			{
				UInvadedData* InvadedData = InvadedGS->GetPlayerStates()[Pair.Key];
				InvadedData->SetRemainingMoves(0);
				InvadedData->SetOrder(i);
				i++;
				TempArray.Add(InvadedData);
			}

			InvadedGS->SetPlayerStates(TempArray);

			PlayerStates = TempArray;

			TArray<AActor*> Actors;

			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInvadedPlayer::StaticClass(), Actors);

			for (AActor* Actor : Actors)
			{
				AInvadedPlayer* InvadedPlayer = Cast<AInvadedPlayer>(Actor);
				if (InvadedPlayer)
				{
					InvadedPlayer->RemoveGameStartUI();
				}
			}
			NextTurn();
		}
		
		
		
	}
	
}
void AInvadedGM::DetermineFightWinner()
{
	if (GetCurrentFightGame())
	{
		if (GetCurrentFightGame()->GetFirstFighter() && GetCurrentFightGame()->GetSecondFighter())
		{
			bSwitchPlayers = false;
			bIsDraw = false;
			UE_LOG(LogTemp, Warning, TEXT("Determine Fight Winner"));

			AActor* Fighter1Temp = GetCurrentFightGame()->GetFirstFighter()->GetOwner();
			AActor* Fighter2Temp = GetCurrentFightGame()->GetSecondFighter()->GetOwner();

			IFightInterface* Fighter1Interface = Cast<IFightInterface>(Fighter1Temp);
			IFightInterface* Fighter2Interface = Cast<IFightInterface>(Fighter2Temp);
			AInvadedCharacter* Fighter1Casted = Cast<AInvadedCharacter>(Fighter1Temp);
			AInvadedCharacter* Fighter2Casted = Cast<AInvadedCharacter>(Fighter2Temp);

			SetPlayerMode(Fighter1Casted, EPlayerMode::PM_Base);
			SetPlayerMode(Fighter2Casted, EPlayerMode::PM_Base);

			Fighter1Casted->GetInvadedPS()->SetPlayerMode(EPlayerMode::PM_Base);
			Fighter2Casted->GetInvadedPS()->SetPlayerMode(EPlayerMode::PM_Base);

			Fighter1Actor = Fighter1Temp;
			Fighter2Actor = Fighter2Temp;

			if (GetCurrentFightGame()->IsDraw())
			{
			
				Fighter2Casted->GetInvadedMovementComp()->EndFight();
				Fighter1Casted->GetInvadedMovementComp()->EndFight();
			
				Fighter2Interface->Execute_FightDraw(Fighter2Temp, Fighter1Casted);
				Fighter1Interface->Execute_FightDraw(Fighter1Temp, Fighter2Casted);

				bIsDraw = true;

			}
			else if (GetCurrentFightGame()->IsFirstFighterWinner())
			{
				
				FVector Fighter1Location = Fighter1Actor->GetActorLocation();
				FVector Fighter2Location = Fighter2Actor->GetActorLocation();
				ACellBase* Fighter1Cell = Fighter1Casted->GetOversteppedCell();
				ACellBase* Fighter2Cell = Fighter2Casted->GetOversteppedCell();
				Fighter1Casted->TeleportPlayer(Fighter1Casted, Fighter2Location);
				Fighter2Casted->TeleportPlayer(Fighter2Casted, Fighter1Location);

				Fighter1Casted->TeleportPlayer(Fighter1Casted, Fighter2Location);
				Fighter2Casted->TeleportPlayer(Fighter2Casted, Fighter1Location);

				Fighter1Casted->SetOversteppedCell(Fighter2Cell);
				Fighter2Casted->SetOversteppedCell(Fighter1Cell);

				Fighter1Casted->GetInvadedMovementComp()->EndFight();
				Fighter2Casted->GetInvadedMovementComp()->EndFight();

				Fighter1Casted->GetInvadedPS()->SetIsAttacker(false);
				Fighter2Casted->GetInvadedPS()->SetIsAttacker(false);

				Fighter2Interface->Execute_FightLost(Fighter2Temp, Fighter1Casted);
				Fighter1Interface->Execute_FightWon(Fighter1Temp, Fighter2Casted);


				bSwitchPlayers = true;
			}
			else
			{
				Fighter1Casted->GetInvadedMovementComp()->EndFight();
				Fighter2Casted->GetInvadedMovementComp()->EndFight();

				Fighter1Casted->GetInvadedPS()->SetIsAttacker(false);
				Fighter2Casted->GetInvadedPS()->SetIsAttacker(false);

				Fighter2Interface->Execute_FightWon(Fighter2Temp, Fighter1Casted);
				Fighter1Interface->Execute_FightLost(Fighter1Temp, Fighter2Casted);
			}



			GetCurrentFightGame()->EndGame(GetCurrentFightGame()->GetFirstFighter());
			GetCurrentFightGame()->EndGame(GetCurrentFightGame()->GetSecondFighter());
			
		}

	}
}
void AInvadedGM::FinishDetermineFightWinner()
{
	if (Fighter1Actor&&Fighter2Actor)
	{
		
		UE_LOG(LogTemp, Warning, TEXT("Finish Determine Fight Winner"));
		IFightInterface* Fighter1Interface = Cast<IFightInterface>(Fighter1Actor);
		IFightInterface* Fighter2Interface = Cast<IFightInterface>(Fighter2Actor);
		AInvadedCharacter* Fighter1Casted = Cast<AInvadedCharacter>(Fighter1Actor);
		AInvadedCharacter* Fighter2Casted = Cast<AInvadedCharacter>(Fighter2Actor);
		FString PopupValue="";
		if (bSwitchPlayers)
		{
	

			Fighter2Casted->GetFightGameComponent()->FightLost(Fighter1Casted);
			Fighter1Casted->GetFightGameComponent()->FightWon(Fighter2Casted);

			PopupValue = Fighter2Casted->GetInvadedPS()->GetPlayerName() + " won";
		}
		else if (!bIsDraw)
		{
			Fighter1Casted->GetFightGameComponent()->FightLost(Fighter2Casted);
			Fighter2Casted->GetFightGameComponent()->FightWon(Fighter1Casted);

			PopupValue = Fighter1Casted->GetInvadedPS()->GetPlayerName() + " won";
		}
		else
		{
			Fighter1Casted->GetFightGameComponent()->FightDraw(Fighter2Casted);
			Fighter2Casted->GetFightGameComponent()->FightDraw(Fighter1Casted);

			PopupValue = "Draw";
		}




		TArray<AActor*> Actors;

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInvadedPlayer::StaticClass(), Actors);

		for (AActor* Actor : Actors)
		{
			AInvadedPlayer* Player = Cast<AInvadedPlayer>(Actor);
			if (Player)
			{
				if (Player != Fighter1Actor && Player != Fighter2Actor)
				{
					Player->AddPopup(PopupValue);
				}
			}
		}

		//CurrentFightGame->ConditionalBeginDestroy();
		//CurrentFightGame = nullptr;
	}
	
}
void AInvadedGM::SetPlayerFightValue(AInvadedCharacter* Fighter, uint8 Value)
{
	if(CurrentFightGame)
	CurrentFightGame->UpdateScore(Fighter->GetInvadedPS(), Value);

}
void AInvadedGM::SetPlayerMode(AInvadedCharacter* Player,EPlayerMode PlayerMode)
{
	UInvadedData* InvadedPS =Player->GetInvadedPS();
	if (InvadedPS)
	{
		InvadedPS->SetPlayerMode(PlayerMode);
	}
}
void AInvadedGM::AdditionalThrow()
{
	bAdditionalThrow++;
	InvadedGS->GetCurrentPlayer()->SetHasTrownDice(false);

}
void AInvadedGM::ResetPlayerLocations()
{
	if (GridGenerator)
	{
		GridGenerator->SetPlayerStart();
	}
}
void AInvadedGM::ResetFlags()
{
	if (GridGenerator)
	{
		GridGenerator->ResetFlags();
	}
}

void AInvadedGM::AddNextBattleWinner(AInvadedCharacter * Player)
{
	BattleWinners.Add(Player);
}

void AInvadedGM::AddNextBattleLoser(AInvadedCharacter * Player)
{
	
	BattleLosers.Add(Player);
}
void AInvadedGM::SkipPlayerTurn(class AInvadedCharacter* Player)
{
	UInvadedData* InvadedPS = Player->GetInvadedPS();
	if (InvadedPS)
	{
		SkipTurnPlayers.Add(InvadedPS);
	}
}
void AInvadedGM::AddCardsToPlayer()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInvadedCharacter::StaticClass(), Actors);
	for (AActor* Actor : Actors)
	{
		
		AInvadedCharacter* InvadedPlayer = Cast<AInvadedCharacter>(Actor);
		if (InvadedPlayer)
		{
			FActorSpawnParameters SpawnParams;
			for (TSubclassOf<ACardBase> CardClass : AttackCardsClasses)
			{
				ACardBase* Card = GetWorld()->SpawnActor<ACardBase>(CardClass, FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f), SpawnParams);
				if(Card)
				InvadedPlayer->AddCard(Card);
			}
			for (TSubclassOf<ACardBase> CardClass : DefensiveCardsClasses)
			{
				ACardBase* Card = GetWorld()->SpawnActor<ACardBase>(CardClass, FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f), SpawnParams);
				if(Card)
				InvadedPlayer->AddCard(Card);
			}
		}
	}
}
void AInvadedGM::AddFlagsToPlayers(TArray<class AFlag*> FlagArray)
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInvadedCharacter::StaticClass(), Actors);
	UE_LOG(LogTemp, Warning, TEXT("NumberOfFlags %d"), Flags.Num());
	for (AActor* Actor : Actors)
	{
		AInvadedCharacter* InvadedCharacter = Cast<AInvadedCharacter>(Actor);
		if (InvadedCharacter)
		{
			for (AFlag* Flag : Flags)
			{
				UInvadedData* InvadedPS = InvadedCharacter->GetInvadedPS();
				if (InvadedPS)
				{

					if (Flag->GetFirstOwner()==InvadedCharacter && InvadedCharacter->GetFlags().Num()==0)
					{
						InvadedCharacter->AddFlag(Flag);
						if (Flag->GetCornerCell())
						{
							Flag->GetCornerCell()->RemoveFlags();
						}
						
						break;
					}
				}

			}	
		}	
	}
}
void AInvadedGM::EndGame()
{
	GameEnded = true;

	UE_LOG(LogTemp, Warning, TEXT("ServerTravel"));

	FString MapName = "MainMenu";

	TArray<AActor*> Actors;
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInvadedCharacter::StaticClass(), Actors);
	for (AActor* Actor : Actors)
	{
		AInvadedCharacter* InvadedCharacter = Cast<AInvadedCharacter>(Actor);
	
		if (InvadedCharacter)
		{
			UInvadedData* InvadedPS = InvadedCharacter->GetInvadedPS();
			if (InvadedPS)
			{
				InvadedPS->SetTurnScore(InvadedGS->GetCurrentTurn());
				UpdateLeaderboardPlayerData(InvadedPS);
			}
				

			InvadedCharacter->RemoveFlags();
			InvadedCharacter->GameWon();

			if (InvadedCharacter->IsA<AInvadedPlayer>())
			{
				APlayerState* PlayerState = InvadedCharacter->GetPlayerState();

				if (PlayerState)
				{
					int32 NewValue = InvadedGS->GetCurrentTurn();
				
					FOnlineLeaderboardWrite WriteObject;
					WriteObject.LeaderboardNames.Add("Score");
					WriteObject.RatedStat = "Score";
					WriteObject.DisplayFormat = ELeaderboardFormat::Number;
					WriteObject.SortMethod = ELeaderboardSort::Descending;
					WriteObject.UpdateMethod = ELeaderboardUpdateMethod::Force;
					WriteObject.IncrementIntStat("Score", NewValue);


					TSharedPtr<const FUniqueNetId> UserId = PlayerState->UniqueId.GetUniqueNetId();
					if (UserId.IsValid())
					{
						if (IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::IsLoaded() ? IOnlineSubsystem::Get() : nullptr)
						{
							IOnlineLeaderboardsPtr Leaderboards = OnlineSub->GetLeaderboardsInterface();
							if (Leaderboards.IsValid())
							{
								Leaderboards->WriteLeaderboards(PlayerState->SessionName, *UserId, WriteObject);

								Leaderboards->FlushLeaderboards(PlayerState->SessionName);
							}
						}
					}

				}

			}
		}
		
	}
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFlag::StaticClass(), Actors);

	for (int32 i = 0; i < Actors.Num(); ++i)
	{
		AFlag* Flag = Cast<AFlag>(Actors[i]);
		if (Flag)
		{
			Flag->Destroy();
		}

	}

	Actors.Empty();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpectatorPlayer::StaticClass(), Actors);
	for (AActor* Actor : Actors)
	{
		ASpectatorPlayer* InvadedSpectator = Cast<ASpectatorPlayer>(Actor);
		if (InvadedSpectator)
			InvadedSpectator->GameWon();

	}

	//GetWorld()->ServerTravel(FString(MapName));
}
FString AInvadedGM::GenerateActorName()
{
	AINames.AddUnique("Jeryl");
	AINames.AddUnique("Adam");
	AINames.AddUnique("Bob");
	AINames.AddUnique("Mike");

	int32 RandomIndex =int32(FMath::FRandRange(0.0f, AINames.Num() - 1));
	FString Value = AINames[RandomIndex];
	AINames.Remove(Value);
	return Value;
}

void AInvadedGM::SetStartWithFlags(bool Value)
{
	bStartWithFlag = Value;
}
void AInvadedGM::SetStartWithCards(bool Value)
{
	bStartWithCards = Value;
}
void AInvadedGM::SetStartWithFight(bool Value)
{
	bStartWithFight = Value;
}
void AInvadedGM::SetUseAllFightingTypes(bool Value)
{
	bUseAllFightingTypes = Value;
}

void AInvadedGM::ThrowDiceSpecified(UInvadedData * InvadedData)
{
	if (DicePod)
	{
		DicePod->SpawnDiceSpecified(InvadedData);
	}
}

bool AInvadedGM::StartFight(UInvadedData* Fighter1,UInvadedData* Fighter2)
{
	if (FightGameTypes.Num()&&Fighter1&&Fighter2)
	{
		if (CurrentFightGame)
			CurrentFightGame->Destroy();

		TArray<AActor*> Actors;

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInvadedPlayer::StaticClass(), Actors);

		for (AActor* Actor : Actors)
		{
			AInvadedPlayer* Player = Cast<AInvadedPlayer>(Actor);
			if (Player)
			{
				if (Player->GetInvadedPS() != Fighter1 && Player->GetInvadedPS() != Fighter2)
				{
					Player->AddFightPopup(Fighter1->GetPlayerName(), Fighter2->GetPlayerName());
				}
			}
		}


		if (bUseAllFightingTypes)
		{
			
			int32 Index = FMath::RandRange(0, FightGameTypes.Num() - 1);
			CurrentFightGame = GetWorld()->SpawnActor<AFightGame>(FightGameTypes[Index], FTransform(), FActorSpawnParameters());

		}
		else
		{
			EBattleType BattleType = InvadedGS->GetBattleType();
			if (BattleType == EBattleType::Both)
			{
				int32 Index = FMath::RandRange(0, FightGameTypes.Num() - 1);
				CurrentFightGame = GetWorld()->SpawnActor<AFightGame>(FightGameTypes[Index], FTransform(), FActorSpawnParameters());
			}
			else if (BattleType == EBattleType::Roll)
			{
				CurrentFightGame = GetWorld()->SpawnActor<AFightGame>(FightGameTypes[0], FTransform(), FActorSpawnParameters());
			}
			else
			{
				CurrentFightGame = GetWorld()->SpawnActor<AFightGame>(FightGameTypes[1], FTransform(), FActorSpawnParameters());
			}
			
		}
		
		ForceNetUpdate();

		if (BattleWinners.Find(Cast<APawn>(Fighter1->GetOwner())) != INDEX_NONE)
		{
			BattleWinners.Remove(Cast<APawn>(Fighter1->GetOwner()));

			CurrentFightGame->SetGameResults(Fighter1, Fighter2, true);

			CurrentFightGame = nullptr;

			return false;
		}
		if (BattleWinners.Find(Cast<APawn>(Fighter2->GetOwner())) != INDEX_NONE)
		{
			BattleWinners.Remove(Cast<APawn>(Fighter2->GetOwner()));

			CurrentFightGame->SetGameResults(Fighter2, Fighter1, false);

			CurrentFightGame = nullptr;

			return false;
		}
		if (BattleLosers.Find(Cast<APawn>(Fighter1->GetOwner())) != INDEX_NONE)
		{
			BattleLosers.Remove(Cast<APawn>(Fighter1->GetOwner()));

			CurrentFightGame->SetGameResults(Fighter2, Fighter1, false);

			CurrentFightGame = nullptr;

			return false;
		}
		if (BattleLosers.Find(Cast<APawn>(Fighter2->GetOwner())) != INDEX_NONE)
		{
			BattleLosers.Remove(Cast<APawn>(Fighter2->GetOwner()));

			CurrentFightGame->SetGameResults(Fighter1, Fighter2, true);

			CurrentFightGame = nullptr;

			return false;
		}
		
		if (CurrentFightGame)
		{
			UE_LOG(LogTemp, Warning, TEXT("Start Fight %s %s"), *Fighter1->GetOwner()->GetName(), *Fighter2->GetOwner()->GetName());
			UE_LOG(LogTemp, Warning, TEXT("CurrentFightGame %s"), *CurrentFightGame->GetName());
		
			CurrentFightGame->Init(Fighter1);
			CurrentFightGame->Init(Fighter2);

			return true;
		}
	}

	return false;
	
	
}

