// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/InvadedMenuState.h"
#include "MainGameWidget.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API UMainGameWidget : public UInvadedMenuState
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
		void ToggleDicePod();
	UFUNCTION(BlueprintImplementableEvent)
		void Update();
};
