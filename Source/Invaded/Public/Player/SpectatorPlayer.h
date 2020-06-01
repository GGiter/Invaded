// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/CameraInterface.h"
#include "Components/ActorComponent.h"
#include "CinematicCamera/Public/CineCameraComponent.h"
#include "Camera/CameraComponent.h"
#include "SpectatorPlayer.generated.h"

UCLASS()
class INVADED_API ASpectatorPlayer : public APawn , public ICameraInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
		void ToggleHUD();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UInvadedData* InvadedPS;
	// Sets default values for this pawn's properties
	ASpectatorPlayer();
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	UFUNCTION(BlueprintCallable)
		UInvadedData* GetInvadedPS() const { return InvadedPS; }

	void SetInvadedPS(UInvadedData* Value);

	virtual void PossessedBy(AController* NewController) override;
protected:

	UPROPERTY(BlueprintReadWrite)
		class UMainGameWidget* MainGameWidget;

	UFUNCTION(BlueprintImplementableEvent)
		void BP_ToggleHUD();

	UFUNCTION(BlueprintImplementableEvent)
		void BP_CollapseHUD();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCineCameraComponent* TopDownCameraComponent;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	UFUNCTION(Client, Reliable, WithValidation)
		void ClientPossessedBy(AController* NewController);

	UFUNCTION(BlueprintPure)
		class UInvadedStateMachine* GetStateMachine() const { return UIStateMachine; }
public:	

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Camera")
		void Zoom(const float& Amount);
	virtual void Zoom_Implementation(const float& Amount);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Camera")
		void MoveForward(const float& Amount);
	virtual void MoveForward_Implementation(const float& Amount);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Camera")
		void MoveRight(const float& Amount);
	virtual void MoveRight_Implementation(const float& Amount);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Camera")
		void Rotate(const float& Amount);
	virtual void Rotate_Implementation(const float& Amount);
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
		TSubclassOf<class UInvadedStateMachine> UIStateMachineClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
		TSubclassOf<class UInvadedStateMachine> UIPauseClass;
	

	class UInvadedStateMachine* UIStateMachine;
	class UInvadedStateMachine* UIPause;
	

	void ToggleChatWindow();
	void TogglePauseUI();

	void ToggleDicePod();

	UFUNCTION(Client,Reliable,WithValidation)
	void ToggleDicePodClient();
	void GameWon();
	UFUNCTION(Client, Reliable, WithValidation)
	void GameWonClient();
	
	UPROPERTY(EditDefaultsOnly, Category = "Zoom")
		float NewDesiredArmLength=600.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Zoom")
		float MinZoom;
	UPROPERTY(EditDefaultsOnly, Category = "Zoom")
		float MaxZoom;
};
