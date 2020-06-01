// Fill out your copyright notice in the Description page of Project Settings.

#include "WordGame.h"
#include "InvadedGM.h"
#include "InvadedPlayer.h"
#include "FightGameComponent.h"
#include "FileHelper.h"

AWordGame::AWordGame()
{
	WidgetStateID = "UIGameWord";

	GameTime = 10.0f;
	NumberOfTries = 3;

	AISpeed = 0.1f;
	AIFailRate = 0.5f;
}
int32 AWordGame::GetEnemyRemainingTries(class UInvadedData* CallingPlayer)
{
	if (FirstFighter&&CallingPlayer&&SecondFighter)
	{
		if (CompareFighters(CallingPlayer, FirstFighter))
		{
			return SecondFighterHealth;
		}

		return FirstFighterHealth;
	}
	return 0;
}
void AWordGame::UpdateScore(UInvadedData * InvadedPS, int32 Score)
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
		if (CompareFighters(InvadedPS,FirstFighter))
		{
			FirstFighterScore+=Score;
		}
		else if (CompareFighters(InvadedPS, SecondFighter))
		{
			SecondFighterScore += Score;
		}
	}

	Super::UpdateScore(InvadedPS, Score);
}

void AWordGame::DetermineFightWinner()
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

void AWordGame::Init(UInvadedData * InvadedPS)
{
	if (InvadedPS)
	{
		InvadedPS->SetPlayerMode(EPlayerMode::PM_Fight);

		if (bFirstPlayer)
		{
			SecondFighter = InvadedPS;

			SecondFighterScore = -1;

			bSecondPlayer = true;

			AInvadedPlayer* Player = Cast<AInvadedPlayer>(SecondFighter->GetOwner());
			if (Player)
			{
				Player->GetFightGameComponent()->StartWordGame(5,GameTime,NumberOfTries);
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
				Player->GetFightGameComponent()->StartWordGame(5,GameTime,NumberOfTries);
			}
		}
	}
}
void AWordGame::BeginPlay()
{
	Super::BeginPlay();

	SetupData();
}
void AWordGame::SetupData()
{
	FString GameDir = FPaths::ProjectDir();
	FString CompleteFilePath = GameDir + "GameWord" + ".txt";

	FString FileData = "";
	FFileHelper::LoadFileToString(FileData, *CompleteFilePath);

	if (!FileData.IsEmpty())
	{
		//WordList.Empty();
		//FileData.ParseIntoArray(WordList, TEXT("\n"), true);
	}	
}
void AWordGame::StartGame()
{
	Super::StartGame();

	AIPlayers.Empty();

	SequenceArray.Empty();

	FirstFighterScore = 0;
	SecondFighterScore = 0;

	bFirstPlayer = false;
	bSecondPlayer = false;

	bGameReady = true;

	FirstFighterSeqIndex = -1;

	SecondFighterSeqIndex = -1;

	FirstFighterHealth = NumberOfTries;

	SecondFighterHealth = NumberOfTries;

	SequenceArray.Add(GenerateWord());

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
			GetWorld()->GetTimerManager().SetTimer(GuessWordTimer, this, &AWordGame::GuessWord, AISpeed, true);
		}
	}

	Super::StartGame();
}

FString AWordGame::GetNextSequence(UInvadedData * InvadedPS)
{
	if (CompareFighters(InvadedPS, FirstFighter))
	{
		FirstFighterSeqIndex++;
		if (SequenceArray.Num() > FirstFighterSeqIndex)
		{
			return SequenceArray[FirstFighterSeqIndex];
		}	
		else
		{
			FString Word = GenerateWord();
			SequenceArray.Add(Word);
			return Word;
		}
	}
	else 
	{
		SecondFighterSeqIndex++;
		if (SequenceArray.Num() > SecondFighterSeqIndex)
		{
			return SequenceArray[SecondFighterSeqIndex];
		}
		else
		{
			FString Word = GenerateWord();
			SequenceArray.Add(Word);
			return Word;
		}
	}
	return FString("EmptySequence");
}

void AWordGame::GuessWord()
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
FString AWordGame::GenerateWord()
{
	int32 RandIndex = FMath::RandRange(0, WordList.Num() - 1);
	if (WordList[RandIndex].IsEmpty())
		return "EmptySequence";
	else
		return WordList[RandIndex];
}
void AWordGame::EndGame(UInvadedData * InvadedPS)
{
	GetWorld()->GetTimerManager().ClearTimer(GuessWordTimer);
	Super::EndGame(InvadedPS);
}
void AWordGame::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWordGame, SequenceArray);

}