// Fill out your copyright notice in the Description page of Project Settings.

#include "FightGame.h"
#include "GameModes/InvadedData.h"
#include "InvadedGM.h"
#include "Kismet/GameplayStatics.h"
#include "AI/InvadedAIPlayer.h"
#include "UnrealNetwork.h"
#include "InvadedPlayer.h"


AFightGame::AFightGame()
{
	WidgetStateID = "UIFight";
	bGameReady = false;

	bReplicates = true;
}
void AFightGame::Init(UInvadedData * InvadedPS)
{
	if (InvadedPS)
	{
		InvadedPS->SetPlayerMode(EPlayerMode::PM_Fight);

		if (bFirstPlayer)
		{
			SecondFighter = InvadedPS;

			SecondFighterScore = -1;

			bSecondPlayer = true;
		}
		else
		{
			FirstFighter = InvadedPS;

			FirstFighterScore = -1;

			bFirstPlayer = true;
		}
	}
	
}
class UInvadedData* AFightGame::GetEnemy(UInvadedData* CallingPlayer)
{
	if (CallingPlayer == FirstFighter)
	{
		return GetSecondFighter();
	}

	return GetFirstFighter();
}
int32 AFightGame::GetEnemyRemainingTries(UInvadedData* CallingPlayer)
{
	return 0;
}
FString AFightGame::GetEnemyName(UInvadedData * CallingPlayer)
{
	if (FirstFighter&&SecondFighter&&CallingPlayer)
	{
		if (CompareFighters(FirstFighter, CallingPlayer))
		{
			return SecondFighter->GetPlayerName();
		}
		else
		{
			return FirstFighter->GetPlayerName();
		}
	}
	
	return FString();
}
int32 AFightGame::GetPlayerScore(UInvadedData * CallingPlayer)
{
	if (FirstFighter&&SecondFighter&&CallingPlayer)
	{
		if (CompareFighters(FirstFighter, CallingPlayer))
		{
			return FirstFighterScore;
		}
		else
		{
			return SecondFighterScore;
		}
	}

	return 0;
}
int32 AFightGame::GetEnemyScore(class UInvadedData* CallingPlayer)
{
	if (FirstFighter&&SecondFighter&&CallingPlayer)
	{
		if (CompareFighters(FirstFighter, CallingPlayer))
		{
			return SecondFighterScore;
		}
		else
		{
			return FirstFighterScore;
		}
	}

	return 0;
}
bool AFightGame::CompareFighters(class UInvadedData* Fighter1, class UInvadedData* Fighter2)
{
	if (Fighter1&&Fighter2)
	{
		if (Fighter1->GetOwner() == Fighter2->GetOwner())
			return true;
	}
	
	return false;
}
void AFightGame::UpdatePlayersData()
{
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),AInvadedCharacter::StaticClass(), OutActors);
	for (AActor* Actor : OutActors)
	{
		AInvadedCharacter* Character = Cast<AInvadedCharacter>(Actor);
		if (Character)
		{
			if (CompareFighters(Character->GetInvadedPS(), FirstFighter) || CompareFighters(Character->GetInvadedPS(), SecondFighter))
			{
				Character->GetFightGameComponent()->UpdateData();
			}	
		}
	}
}
void AFightGame::InitReached(UInvadedData * InvadedPS)
{	
	UE_LOG(LogTemp, Warning, TEXT("InitReached"));
	if (CompareFighters(InvadedPS, FirstFighter)|| CompareFighters(InvadedPS, SecondFighter))
	{
		UE_LOG(LogTemp, Warning, TEXT("StateID %s"), *InvadedPS->GetOwner()->GetName());
		AInvadedPlayer* Player = Cast<AInvadedPlayer>(InvadedPS->GetOwner());
		if (Player)
		{	
			Player->AddFightUI(WidgetStateID);
		}
	}
	if (!bGameReady)
	{
		bGameReady = true;
	}
	else
	{
		StartGame();
	}
}
void AFightGame::StartGame()
{
	UE_LOG(LogTemp, Warning, TEXT("Start fight game."));
	UpdatePlayersData();

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInvadedPlayer::StaticClass(), OutActors);


	if (!FirstFighter->GetOwner()->IsA<AInvadedAIPlayer>() || !SecondFighter->GetOwner()->IsA<AInvadedAIPlayer>())
	{
		for (AActor* Actor : OutActors)
		{
			AInvadedPlayer* Player = Cast<AInvadedPlayer>(Actor);
			if (Player)
			{
				if (!CompareFighters(Player->GetInvadedPS(), FirstFighter) && !CompareFighters(Player->GetInvadedPS(), SecondFighter))
				{
					Player->AddFightUI("FightSpectator");
				}
			}
		}
	}
}
void AFightGame::UpdateScore(UInvadedData * InvadedPS, int32 Score)
{
	UpdatePlayersData();
}

void AFightGame::DetermineFightWinner()
{
	if (FirstFighterScore == SecondFighterScore)
	{
		Winner = FirstFighter;
		Loser = FirstFighter;
	}
	else if (FirstFighterScore > SecondFighterScore)
	{
		Winner = FirstFighter;
		Loser = SecondFighter;
	}
	else
	{
		Winner = SecondFighter;
		Loser = FirstFighter;
	}
	

	

	AInvadedGM* InvadedGM = GetWorld()->GetAuthGameMode<AInvadedGM>();
	if (InvadedGM&&bGameReady)
	{
		bFirstPlayer = false;
		bSecondPlayer = false;
		bGameReady = false;
		InvadedGM->DetermineFightWinner();
	}
	
}

void AFightGame::EndGame(UInvadedData * InvadedPS)
{
	FirstFighterScore = -1;
	SecondFighterScore = -1;

	if (CompareFighters(InvadedPS, FirstFighter))
	{
		bFirstPlayer = false;
	}
	if (CompareFighters(InvadedPS, SecondFighter))
	{
		bSecondPlayer = false;
	}
	
	bGameReady = false;

	if (CompareFighters(InvadedPS, FirstFighter)|| CompareFighters(InvadedPS, SecondFighter))
	{
		if (InvadedPS->GetOwner()->IsA<AInvadedAIPlayer>())
		{
			EndGameReached(InvadedPS);
		}
	}

	if (!FirstFighter->GetOwner()->IsA<AInvadedAIPlayer>() || !SecondFighter->GetOwner()->IsA<AInvadedAIPlayer>())
	{
		TArray<AActor*> OutActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInvadedPlayer::StaticClass(), OutActors);

		for (AActor* Actor : OutActors)
		{
			AInvadedPlayer* Player = Cast<AInvadedPlayer>(Actor);
			if (Player)
			{
				if (!CompareFighters(Player->GetInvadedPS(), FirstFighter) && !CompareFighters(Player->GetInvadedPS(), SecondFighter))
				{
					Player->RemoveFightUI("FightSpectator");
				}
			}
		}
	}
	
	
}

void AFightGame::EndGameReached(UInvadedData * InvadedPS)
{
	UE_LOG(LogTemp, Warning, TEXT("End Game Reached"));
	if (!bFirstPlayer)
	{
		bFirstPlayer = true;
	}
	else 
	{
		bSecondPlayer = true;
	}
	if (bFirstPlayer&&bSecondPlayer&&FirstFighter&&SecondFighter)
	{

		AInvadedPlayer* FirstPlayer = Cast<AInvadedPlayer>(FirstFighter->GetOwner());
		if (FirstPlayer)
		{
			FirstPlayer->RemoveFightUI(WidgetStateID);
			
		}

		AInvadedPlayer* SecondPlayer = Cast<AInvadedPlayer>(SecondFighter->GetOwner());
		if (SecondPlayer)
		{
			SecondPlayer->RemoveFightUI(WidgetStateID);
			
		}
		FirstFighter->SetPlayerMode(EPlayerMode::PM_Base);
		SecondFighter->SetPlayerMode(EPlayerMode::PM_Base);

		bFirstPlayer = false;
		bSecondPlayer = false;
		AInvadedGM* InvadedGM = GetWorld()->GetAuthGameMode<AInvadedGM>();
		if (InvadedGM)
		{
			InvadedGM->FinishDetermineFightWinner();
		}
		FirstFighterScore = -1;
		SecondFighterScore = -1;
	}


}
void AFightGame::SetGameResults(UInvadedData * WinnerValue, UInvadedData * LoserValue, bool bIsAttackWinner)
{
	if (bIsAttackWinner)
	{
		FirstFighter = WinnerValue;
		SecondFighter = LoserValue;

		FirstFighterScore = 10;
		SecondFighterScore = 0;
	}
	else
	{
		FirstFighter = LoserValue;
		SecondFighter = WinnerValue;

		FirstFighterScore = 0;
		SecondFighterScore = 10;
	}

	DetermineFightWinner();
	
}
void AFightGame::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFightGame, FirstFighter);
	DOREPLIFETIME(AFightGame, SecondFighter);
	DOREPLIFETIME(AFightGame, Winner);
	DOREPLIFETIME(AFightGame, Loser);
	DOREPLIFETIME(AFightGame,bGameReady);
	DOREPLIFETIME(AFightGame, FirstFighterScore);
	DOREPLIFETIME(AFightGame, SecondFighterScore);
}