// Fill out your copyright notice in the Description page of Project Settings.

#include "GameWordWidget.h"
#include "InvadedGM.h"
#include "Engine/World.h"
#include "InvadedData.h"
#include "MiniGames/WordGame.h"
#include "InvadedPlayer.h"
#include "Kismet/GameplayStatics.h"

void UGameWordWidget::SentenceSucceed(int32 Lenght)
{
	AInvadedPlayer* Player = Cast<AInvadedPlayer>(GetOwningPlayerPawn());
	if (Player)
	{
		UInvadedData* InvadedData = Player->GetInvadedPS();
		AInvadedGM* InvadedGM = GetWorld()->GetAuthGameMode<AInvadedGM>();
		if (InvadedGM)
		{
			AWordGame* WordGame = Cast<AWordGame>(InvadedGM->GetCurrentFightGame());
			if (WordGame)
			{
				WordGame->UpdateScore(InvadedData, Lenght);
			}
		}
		
	}
}

void UGameWordWidget::SentenceFail()
{
	AInvadedPlayer* Player = Cast<AInvadedPlayer>(GetOwningPlayerPawn());
	if (Player)
	{
		UInvadedData* InvadedData = Player->GetInvadedPS();
		AInvadedGM* InvadedGM = GetWorld()->GetAuthGameMode<AInvadedGM>();
		if (InvadedGM)
		{
			AWordGame* WordGame = Cast<AWordGame>(InvadedGM->GetCurrentFightGame());
			if (WordGame)
			{
				WordGame->UpdateScore(InvadedData, -1);
			}
		}

	}
}

void UGameWordWidget::TimeIsUp()
{
	AInvadedPlayer* Player = Cast<AInvadedPlayer>(GetOwningPlayerPawn());
	if (Player)
	{
		UInvadedData* InvadedData = Player->GetInvadedPS();
		AInvadedGM* InvadedGM = GetWorld()->GetAuthGameMode<AInvadedGM>();
		if (InvadedGM)
		{
			AWordGame* WordGame = Cast<AWordGame>(InvadedGM->GetCurrentFightGame());
			if (WordGame)
			{
				if (WordGame->GetFirstFighter() == InvadedData)
				{
					WordGame->DetermineFightWinner();
				}
			
			}
		}

	}
}

FString UGameWordWidget::NextSequence()
{
	AInvadedPlayer* Player = Cast<AInvadedPlayer>(GetOwningPlayerPawn());
	if (Player)
	{
		UInvadedData* InvadedData = Player->GetInvadedPS();
		AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
		if (InvadedGM)
		{
			AWordGame* WordGame = Cast<AWordGame>(InvadedGM->GetCurrentFightGame());
			if (WordGame)
			{
				return WordGame->GetNextSequence(InvadedData);
			}
		}
		else
		{
			return FString("Fail2");
		}

	}
	return FString("Fail");
}
