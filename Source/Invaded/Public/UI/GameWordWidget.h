// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/FightMenuState.h"
#include "GameWordWidget.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API UGameWordWidget : public UFightMenuState
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
		void SentenceSucceed(int32 Lenght);
	UFUNCTION(BlueprintCallable)
		void SentenceFail();
	UFUNCTION(BlueprintCallable)
		void TimeIsUp();
	UFUNCTION(BlueprintCallable)
		FString NextSequence();
	UFUNCTION(BlueprintImplementableEvent)
		void UpdateSequence(const FString & Value);
};
