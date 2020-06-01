// Fill out your copyright notice in the Description page of Project Settings.

#include "MenuGM.h"
#include "GameFramework/GameState.h"
#include "MenuPS.h"

void AMenuGM::SetPlayerColor(int32 PlayerID, FLinearColor Color)
{
	AMenuPS* MenuPS = Cast<AMenuPS>(GameState->PlayerArray[PlayerID]);

	if (MenuPS)
	{
		MenuPS->SetPlayerColor(Color);
	}
}

void AMenuGM::SetFlagColor(int32 PlayerID, FLinearColor Color)
{
	AMenuPS* MenuPS = Cast<AMenuPS>(GameState->PlayerArray[PlayerID]);

	if (MenuPS)
	{
		MenuPS->SetFlagColor(Color);
	}
}
