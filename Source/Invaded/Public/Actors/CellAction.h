// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/CellBase.h"
#include "CellAction.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API ACellAction : public ACellBase
{
	GENERATED_BODY()
public:
	virtual void Activate(APawn*Caller) override;

	virtual void OnInteract_Implementation(APawn*Caller) override;
protected:
	UFUNCTION(Server, Reliable, WithValidation)
		void DrawCard();

	UPROPERTY(Replicated)
		class ACardBase* CardRef;

	
	
	
};
