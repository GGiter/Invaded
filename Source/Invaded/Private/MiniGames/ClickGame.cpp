// Fill out your copyright notice in the Description page of Project Settings.

#include "ClickGame.h"
#include "InvadedGM.h"
#include "InvadedPlayer.h"
#include "FightGameComponent.h"

AClickGame::AClickGame()
{
	WidgetStateID = "UIGameClick";

	GameTime = 10.0f;
	NumberOfTries = 3;
	MaxTarget = 6;

	AISpeed = 0.1f;
	AIFailRate = 0.5f;
}

void AClickGame::UpdateScore(UInvadedData * InvadedPS, int32 Score)
{
	if (Score == -1)
	{
		if (CompareFighters(InvadedPS, FirstFighter))
		{
			FirstFighterHealth--;
		}
		else if (CompareFighters(InvadedPS, SecondFighter))
		{
			SecondFighterHealth--;
		}
		if (SecondFighterHealth <= 0 || FirstFighterHealth <= 0)
		{
			DetermineFightWinner();
		}
	}
	else
	{
		if (CompareFighters(InvadedPS, FirstFighter))
		{
			FirstFighterScore += Score;
		}
		else if (CompareFighters(InvadedPS,SecondFighter))
		{
			SecondFighterScore += Score;
		}
	}

	Super::UpdateScore(InvadedPS, Score);
}

void AClickGame::DetermineFightWinner()
{
	if (FirstFighterHealth <= 0)
	{
		FirstFighterScore = -10;
	}

	if (SecondFighterHealth <= 0)
	{
		SecondFighterScore = -10;
	}

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
		bGameReady = false;
		InvadedGM->DetermineFightWinner();
	}
}

void AClickGame::Init(UInvadedData * InvadedPS)
{
	if (InvadedPS)
	{

		if (bFirstPlayer)
		{
			SecondFighter = InvadedPS;

			SecondFighterScore = -1;

			bSecondPlayer = true;

			AInvadedPlayer* Player = Cast<AInvadedPlayer>(SecondFighter->GetOwner());
			if (Player)
			{
				if (Player->GetFightGameComponent())
				Player->GetFightGameComponent()->StartClickGame(5, GameTime, NumberOfTries);
			}
		}
		else
		{
			FirstFighter = InvadedPS;

			FirstFighterScore = -1;

			bFirstPlayer = true;

			AInvadedPlayer* Player = Cast<AInvadedPlayer>(FirstFighter->GetOwner());
			if (Player)
			{
				if(Player->GetFightGameComponent())
				Player->GetFightGameComponent()->StartClickGame(5, GameTime, NumberOfTries);
			}
		}
	}
}

void AClickGame::EndGame(UInvadedData * InvadedPS)
{
	GetWorld()->GetTimerManager().ClearTimer(ClickPartTimer);
	Super::EndGame(InvadedPS);
}

void AClickGame::StartGame()
{
	Super::StartGame();

	AIPlayers.Empty();

	FirstFighterScore = -1;
	SecondFighterScore = -1;

	bFirstPlayer = false;
	bSecondPlayer = false;

	bGameReady = true;


	FirstFighterSeqIndex = -1;

	SecondFighterSeqIndex = -1;


	FirstFighterHealth = NumberOfTries;

	SecondFighterHealth = NumberOfTries;

	TargetArray.Add(GenerateNextTarget());


	if (FirstFighter->GetOwner()->IsA<AInvadedAIPlayer>())
	{
		AIPlayers.Add(FirstFighter);
	}
	if (SecondFighter->GetOwner()->IsA<AInvadedAIPlayer>())
	{
		AIPlayers.Add(SecondFighter);
	}

	if (AIPlayers.Num())
	{
		if (AIPlayers.Num() == 2)
		{
			if (FMath::FRand() > 0.5f)
			{
				for (int32 i = 0; i < NumberOfTries; ++i)
				{
					UpdateScore(AIPlayers[0], -1);
				}
			}
			else
			{
				for (int32 i = 0; i < NumberOfTries; ++i)
				{
					UpdateScore(AIPlayers[1], -1);
				}
			}
		}
		else
		{
			GetWorld()->GetTimerManager().SetTimer(ClickPartTimer, this, &AClickGame::ClickPart, AISpeed, true);
		}
		
	}
}

int32 AClickGame::GetNextTarget(UInvadedData * InvadedPS)
{
	if (CompareFighters(InvadedPS, FirstFighter))
	{
		FirstFighterSeqIndex++;
		if (TargetArray.Num() > FirstFighterSeqIndex)
		{
			return TargetArray[FirstFighterSeqIndex];
		}
		else
		{
			TargetArray.Add(GenerateNextTarget());
			return TargetArray.Last();
		}
	}
	else if (CompareFighters(InvadedPS, SecondFighter))
	{
		SecondFighterSeqIndex++;
		if (TargetArray.Num() > SecondFighterSeqIndex)
		{
			return TargetArray[SecondFighterSeqIndex];
		}
		else
		{
			TargetArray.Add(GenerateNextTarget());
			return TargetArray.Last();
		}
	}
	return 0;
}

void AClickGame::ClickPart()
{
	for (UInvadedData* AIPlayer : AIPlayers)
	{
	
		if (FMath::FRand() > AIFailRate)
		{
			UpdateScore(AIPlayer, 1);
		}
		else
		{
			UpdateScore(AIPlayer, -1);
		}
	}
}

int32 AClickGame::GenerateNextTarget()
{
	return FMath::RandRange(0,MaxTarget-1);
}
