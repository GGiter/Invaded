// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MenuPC.h"
#include "LobbyGS.h"
#include "LobbyPC.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API ALobbyPC : public AMenuPC
{
	GENERATED_BODY()
public:
	ALobbyPC();
	virtual void BeginPlay() override;
	UFUNCTION(Client, Reliable, WithValidation)
		void UpdatePlayerList();

	UFUNCTION(Client, Reliable, WithValidation)
	void LockButtons();
	UFUNCTION(Client, Reliable, WithValidation)
	void SetRemainingTime(float Time);
	UFUNCTION(Client,Reliable,WithValidation)
	void AddLobbyUI(TSubclassOf<class UUserWidget> LobbyUIClass);
	virtual void RemoveWidgets() override;
	UFUNCTION(BlueprintCallable)
		void SetPlayerReady(const bool& Value);

	UFUNCTION(Server, Reliable, WithValidation)
		void SetPlayerReadyServer(bool Value);

	class UInvadedStateMachine* StateMachineWidget;

	UFUNCTION(BlueprintCallable)
		void UpdateServerPlayerArray();	
	UFUNCTION(Server, Reliable, WithValidation)
		void UpdateServerPlayerArrayServer();

	UFUNCTION(BlueprintPure)
		TArray<class APlayerState*> GetServerPlayerArray() const { return ServerPlayerArray; }

	UFUNCTION(BlueprintCallable)
		void SetMapInfo(FMapInfo MapInfo);

	UFUNCTION(BlueprintCallable,Category="Session")
		void ExitSession();
protected:
	UFUNCTION(Server, Reliable, WithValidation)
		void ExitSessionServer();
private:
	UFUNCTION(Server, Reliable, WithValidation)
		void SetMapInfoServer(FMapInfo MapInfo);

		class ULobbyWidget* LobbyWidget;
	
		FString TestString;

	UPROPERTY(EditAnywhere, Category = "Camera")
		FString CameraTag;

	UPROPERTY(Replicated)
		TArray<class APlayerState*> ServerPlayerArray;

};
