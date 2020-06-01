// Fill out your copyright notice in the Description page of Project Settings.

#include "RollGame.h"
#include "InvadedData.h"
#include "InvadedCharacter.h"
#include "InvadedAIPlayer.h"

void ARollGame::UpdateScore(class UInvadedData* InvadedPS, int32 Score)
{
	if (CompareFighters(InvadedPS, FirstFighter))
	{
		FirstFighterScore = Score;
	}
	else if (CompareFighters(InvadedPS, SecondFighter))
	{
		SecondFighterScore = Score;
	}

	if (FirstFighterScore > 0  && SecondFighterScore > 0 )
	{
		DetermineFightWinner();
	}

	Super::UpdateScore(InvadedPS, Score);
}
void ARollGame::StartGame()
{
	Super::StartGame();

	bGameReady = true;
	if (FirstFighter!=nullptr&&SecondFighter!=nullptr)
	{
		AInvadedAIPlayer* FirstCharacter = Cast<AInvadedAIPlayer>(FirstFighter->GetOwner());
		if (FirstCharacter)
		{
			FirstCharacter->FinishFightMode();
		}
		AInvadedAIPlayer* SecondCharacter = Cast<AInvadedAIPlayer>(SecondFighter->GetOwner());
		if (SecondCharacter)
		{
			SecondCharacter->FinishFightMode();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Roll Game Star Game Fail"));
	}
	
}
