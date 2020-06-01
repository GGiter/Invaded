// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Cards/CardBase.h"
#include "CardNuke.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API ACardNuke : public ACardBase
{
	GENERATED_BODY()
public:
	ACardNuke();
	virtual void OnInteract_Implementation(AInvadedCharacter*Caller) override;
	virtual void Activate(class AInvadedCharacter* Causer, class AInvadedCharacter* Target) override;
	
	
};
