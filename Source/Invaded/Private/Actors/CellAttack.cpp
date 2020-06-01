// Fill out your copyright notice in the Description page of Project Settings.

#include "CellAttack.h"
#include "Kismet/GameplayStatics.h"
#include "GameModes/InvadedGM.h"
#include "UnrealNetwork.h"
#include "Engine/World.h"
#include "CardBase.h"
#include "InvadedPlayer.h"

ACellAttack::ACellAttack()
{
	CellType = ECellType::CT_Attack;
}
