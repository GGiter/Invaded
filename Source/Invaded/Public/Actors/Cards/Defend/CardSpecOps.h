// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Cards/CardBase.h"
#include "CardSpecOps.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API ACardSpecOps : public ACardBase
{
	GENERATED_BODY()
public:
	ACardSpecOps();
	virtual void OnInteract_Implementation(AInvadedCharacter* Caller) override;
	virtual void Activate(class AInvadedCharacter* Causer, class AInvadedCharacter* Target) override;
	
	
};
