// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MenuGM.h"
#include "MainMenuGM.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API AMainMenuGM : public AMenuGM
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class UUserWidget> UIClass;
	UFUNCTION(BlueprintImplementableEvent)
		void BP_DedicatedServer();
	
	UFUNCTION(BlueprintCallable)
		void ServerTravel();

	UPROPERTY(EditAnywhere, Category = "ServerTravel")
		FString DestinationName;

};
