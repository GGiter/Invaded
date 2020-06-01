// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InvadedUtilityFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API UInvadedUtilityFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "Utility")
		static bool IsWithEditor();
};
