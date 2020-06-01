// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Cards/CardBase.h"
#include "CardNavalStrike.generated.h"

UCLASS()
class INVADED_API ACardNavalStrike : public ACardBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACardNavalStrike();
	virtual void OnInteract_Implementation(AInvadedCharacter*Caller) override;
	virtual void Activate(class AInvadedCharacter* Causer, class AInvadedCharacter* Target) override;

	
	
};
