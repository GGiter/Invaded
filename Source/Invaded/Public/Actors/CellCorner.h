// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/CellBase.h"
#include "CellCorner.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API ACellCorner : public ACellBase
{
	GENERATED_BODY()
public:
	UFUNCTION(Server,Reliable,WithValidation)
	void AddHiddenCell(ACellBase* Cell);
	TArray<ACellBase*> GetHiddenCells() const { return HiddenCells; }
	virtual FVector GetActorLocationTo(AActor* Actor) override;
private:
	UPROPERTY(Replicated)
	TArray<ACellBase*> HiddenCells;
	
};
