// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "LobbyPlayer.generated.h"

UCLASS()
class INVADED_API ALobbyPlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ALobbyPlayer();

	void LockButtons();

	void UpdatePlayerList();

	void ClearWidgets();

	void SetRemainingTime(float Value);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void LoadVariables();

	UFUNCTION(Server, Reliable, WithValidation)
	void LoadVariablesServer();

	void InitializePlayer();

	UFUNCTION(Server,Reliable,WithValidation)
	void InitializePlayerServer();

	UFUNCTION(Client, Reliable, WithValidation)
	void LockButtonsClient();

	UFUNCTION(Client, Reliable, WithValidation)
	void UpdatePlayerListClient();

	UFUNCTION(Client, Reliable, WithValidation)
	void ClearWidgetsClient();

	UFUNCTION(Client, Reliable, WithValidation)
	void SetRemainingTimeClient(float Value);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnClearWidgets();
public:	
	UPROPERTY(EditDefaultsOnly,Category="UI")
		TSubclassOf<class UUserWidget> LobbyUIClass;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
