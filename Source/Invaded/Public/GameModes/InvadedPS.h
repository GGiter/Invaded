// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "InvadedPS.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API AInvadedPS : public APlayerState
{
	GENERATED_BODY()
public:
	virtual void ClientInitialize(AController* C) override;

	virtual void BeginPlay() override;
	
	void LoadVariables(class AInvadedPlayer* Player);

	UFUNCTION(Server,Reliable,WithValidation)
	void LoadVariablesServer(class AInvadedPlayer* Player);
	

	
};
