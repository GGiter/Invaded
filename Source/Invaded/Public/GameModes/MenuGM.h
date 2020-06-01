// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MenuGM.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API AMenuGM : public AGameMode
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
		void SetPlayerColor(int32 PlayerID, FLinearColor Color);
	UFUNCTION(BlueprintCallable)
		void SetFlagColor(int32 PlayerID, FLinearColor Color);
	
	
	
};
