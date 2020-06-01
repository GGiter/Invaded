// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FightGame.h"
#include "RollGame.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API ARollGame : public AFightGame
{
	GENERATED_BODY()
public:
	
		virtual void UpdateScore(class UInvadedData* InvadedPS, int32 Score) override;

		virtual void StartGame() override;
	
};
