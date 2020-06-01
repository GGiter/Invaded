// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbySG.h"



ULobbySG::ULobbySG()
{
	SaveSlotName = TEXT("LobbySaveSlot");
	UserIndex = 0;
	PlayerColor = { 1.0f,1.0f,1.0f };
	NumOfPlayers = 1;
}
