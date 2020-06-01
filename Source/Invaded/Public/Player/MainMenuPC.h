// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MenuPC.h"
#include "Online.h"
#include "MainMenuPC.generated.h"


UCLASS()
class INVADED_API AMainMenuPC : public AMenuPC
{
	GENERATED_BODY()
public:
	AMainMenuPC();
	virtual	void CallServerTravel(const FString& LevelName) override;
	virtual void RemoveWidgets() override;
	UPROPERTY(BlueprintReadWrite)
	class UUserWidget* UIRef;
};
