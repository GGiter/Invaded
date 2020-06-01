// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MenuGM.h"
#include "LobbyGM.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API ALobbyGM : public AMenuGM
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class UUserWidget> LobbyUIClass;
	void LockPlayers();
	UFUNCTION()
	void ServerTravel();

	virtual void BeginPlay() override;

	void AddPlayerArray(class ALobbyPlayer* Player);

	virtual void PostLogin(APlayerController * NewPlayer) override;

	void UpdateInstalledItemIDs();
protected:
	
	TArray<class ALobbyPlayer*> PlayerArray;

	FTimerHandle ServerTravelHandle;
	UPROPERTY(EditAnywhere, Category = "ServerTravel")
		float SecondsTillTravel;
	UPROPERTY(EditAnywhere, Category = "ServerTravel")
		FString DestinationName;


	
};
