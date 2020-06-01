// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MenuPC.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API AMenuPC : public APlayerController
{
	GENERATED_BODY()
public:
	AMenuPC();
	UFUNCTION(BlueprintCallable)
		void SetPlayerColor(FLinearColor Color, int32 PlayerID);
	UFUNCTION(Server, Reliable, WithValidation)
		void SetPlayerColorServer(FLinearColor Color, int32 PlayerID);
	UFUNCTION(BlueprintCallable)
		void SetFlagColor(FLinearColor Color, int32 PlayerID);
	UFUNCTION(Server, Reliable, WithValidation)
		void SetFlagColorServer(FLinearColor Color, int32 PlayerID);
	UFUNCTION(BlueprintCallable)
	virtual	void CallServerTravel(const FString& LevelName);
	UFUNCTION(Server, Reliable, WithValidation)
		void CallServerTravelServer(const FString& LevelName);
	virtual void RemoveWidgets();
	
	
};
