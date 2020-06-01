// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface.h"
#include "HighlightInterface.generated.h"


UENUM(BlueprintType)		
enum class EHighlightState : uint8
{
	HC_Neutral 	UMETA(DisplayName = "Neutral"),
	HC_Negative 	UMETA(DisplayName = "Negative"),
	HC_Special UMETA(DisplayName="Special"),
	HC_Positive	UMETA(DisplayName = "Positive"),
	HC_Inactive UMETA(DisplayName="Inactive")
};

UINTERFACE(BlueprintType)
class INVADED_API UHighlightInterface : public UInterface
{
	GENERATED_BODY()
};

class INVADED_API IHighlightInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void OnBeginFocus();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void OnEndFocus();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void SetHighlightState(EHighlightState Value);

};