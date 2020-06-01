// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "Interface.h"
#include "InvadedAnimInterface.generated.h"

UINTERFACE(BlueprintType)
class INVADED_API UInvadedAnimInterface : public UInterface
{
	GENERATED_BODY()
};

class INVADED_API IInvadedAnimInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void PlayTurnMontage();
};
