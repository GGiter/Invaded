// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InvadedAnimInstance.h"
#include "GameFramework/PlayerController.h"
#include "InvadedPC.generated.h"

/**
 * 
 */

UCLASS()
class INVADED_API AInvadedPC : public APlayerController
{
	GENERATED_BODY()
public:
	AInvadedPC();
	virtual void BeginPlay() override;
	virtual void BeginPlayingState() override;
	void SetMoveDestination(FVector Destination);
	UFUNCTION(Client,Reliable,WithValidation)
	void SetMoveDestinationClient(FVector Destination);
	
	/*Player Locomotion*/
	void SetNewZRotation(float ZRotation);
	UFUNCTION(BlueprintCallable)
		class UInvadedData* GetCurrentGSPlayer() const;
	UFUNCTION(BlueprintCallable)
		int32 GetCurrentTurn() const;
	
	
		void AddLoadingWidget();

	void RemoveLoadingWidget();

	UFUNCTION(Client,Reliable,WithValidation)
	void LookAtPawn();

	void LookAtScreen(); //Looks at loading screen

	UFUNCTION(BlueprintCallable)
		void ThrowDice();
	UFUNCTION(Server,Reliable,WithValidation)
		void ThrowDiceServer();
	UFUNCTION(BlueprintCallable)
		void TogglePauseUI();
	UFUNCTION(BlueprintCallable)
		void ToggleChatWindow();

	void Spectate();

	UFUNCTION(BlueprintCallable)
		void LockButtons();

	UFUNCTION(BlueprintCallable)
		void UnlockButtons();

	virtual void AcknowledgePossession(APawn * P);
protected:
	UPROPERTY(Replicated)
	uint32 bRotateCamera : 1;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	void OnUse();
	UFUNCTION(Server, Reliable, WithValidation)
		void OnUseServer();
	
	
	
	
	

	void ZoomIn();
	void ZoomOut();
	void RotateCamera();
	void StartRotating();
	void StopRotating();
	UFUNCTION(BlueprintCallable, WithValidation, NetMulticast, Unreliable, Category = PlayerAbility)
	void Drop();
	
	void MoveForward(float Value);
	void MoveRight(float Value);

	
	TArray<class ACellBase*> Path;
		
	UPROPERTY(Replicated)
	class AInvadedPlayer* PlayerRef;
	UPROPERTY(Replicated)
	class ASpectatorPlayer* SpectatorPlayerRef;

	UPROPERTY(EditDefaultsOnly, Category = "Spectator")
		TSubclassOf<class ASpectatorPlayer> SpectatorPlayerClass;
	UPROPERTY(EditDefaultsOnly, Category = "Spectator")
		TSubclassOf<class UUserWidget> LoadingWidgetClass;

	class UUserWidget* LoadingWidget;

	float ZoomAmount = 50.0f;
	

	bool bLockButtons = false;

	class UInvadedData* InvadedPS;
};
