// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/InvadedMenuState.h"
#include "FightMenuState.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API UFightMenuState : public UInvadedMenuState
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void FightWon();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void FightLost();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void FightDraw();
};
