// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MenuPS.h"
#include "LobbyPS.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API ALobbyPS : public AMenuPS
{
	GENERATED_BODY()
public:
	void LoadVariables();
	virtual void ClientInitialize(AController* C) override;

};
