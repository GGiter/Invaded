// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "Interface.h"
#include "FightInterface.generated.h"

UINTERFACE(BlueprintType)
class INVADED_API UFightInterface : public UInterface
{
	GENERATED_BODY()
};

class INVADED_API IFightInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void FightWon(class AInvadedCharacter* Enemy);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void FightDraw(class AInvadedCharacter* Enemy);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void FightLost(class AInvadedCharacter* Enemy);

	
};
