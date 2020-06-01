// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "InvadedPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API AInvadedPlayerStart : public APlayerStart
{
	GENERATED_BODY()
public:
	/** Whether players can start at this point */
	UPROPERTY(EditInstanceOnly)
		uint32 bTaken : 1;	
	
	
};
