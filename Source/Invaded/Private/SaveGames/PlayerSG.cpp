// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSG.h"

UPlayerSG::UPlayerSG()
{
	SaveSlotName = TEXT("PlayerSaveSlot");
	UserIndex = 0;
	ZoomSpeed = 1.0f;
	Color1 = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);
	Color2 = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
}