// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InvadedPC.h"
#include "Flag.h"
#include "Environment/GridGenerator.h"
#include "InvadedCharacter.h"
#include "InvadedData.h"
#include "Actors/Cards/CardBase.h"
#include "Interfaces/CameraInterface.h"
#include "TimerManager.h"
#include "CinematicCamera/Public/CineCameraComponent.h"
#include "Camera/CameraComponent.h"
#include "InvadedPlayer.generated.h"


UCLASS()
class INVADED_API AInvadedPlayer : public AInvadedCharacter , public ICameraInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void ToggleHUD();

	UFUNCTION(BlueprintCallable)
	void ThrowDiceSpecified(UInvadedData* Value);
	UFUNCTION(Server, Reliable, WithValidation)
		void ThrowDiceSpecifiedServer(UInvadedData* Value);

	UFUNCTION(BlueprintPure)
		FString GetCurrentFightStateID() const { return CurrentFightStateID; }

	AInvadedCharacter* TempEnemy;
	// Sets default values for this character's properties
	AInvadedPlayer(const FObjectInitializer& ObjectInitializer);

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/

	virtual void CallTurnEnd() override;

	void AddGameUI();

	UFUNCTION(Client,Reliable,WithValidation)
	void AddGameUIClient();

	void AddEndGameUI();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void AddEndGameUIClient();
	
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void SetCameraLocation(FVector Location);
	
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void SetCameraRotation(FRotator Rotation);
	
	virtual void FightWon_Implementation(class AInvadedCharacter* Enemy) override;

	virtual void FightLost_Implementation(class AInvadedCharacter* Enemy) override;
	
	virtual void FightDraw_Implementation(class AInvadedCharacter* Enemy) override;

	virtual void ToggleDicePod() override;

	UFUNCTION(Client, Reliable, WithValidation)
		void ToggleDicePodClient();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fight")
		void FightWon_BP();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fight")
		void FightLost_BP();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fight")
		void FightDraw_BP();


		void PushState(const FString& StateID);

	UFUNCTION(Client, Reliable, WithValidation)
		void PushStateClient(const FString& StateID);

		void AddCardPopUp(const FCardInfo& CardInfo);

		void AddFightPopup(const FString& Player1, const FString& Player2);

		void AddPopup(const FString& Value);
	
	UFUNCTION(Client, Reliable, WithValidation)
		void AddCardPopUpClient(const FCardInfo& CardInfo);

	UFUNCTION(Client, Reliable, WithValidation)
		void AddFightPopupClient(const FString& Player1, const FString& Player2);

	UFUNCTION(Client, Reliable, WithValidation)
		void AddPopupClient(const FString& Value);


	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fight")
		void AddCardPopUp_BP(const FCardInfo& CardInfo);

	virtual void AddCard(class ACardBase* Value) override;

	UFUNCTION(BlueprintCallable)
		void RemoveCardPopup();

		void InitializePlayer();

	UFUNCTION(Server, Reliable, WithValidation)
		void InitializePlayerServer();

	void AddFightUI(const FString& StateID);
		
	UFUNCTION(Client, Reliable, WithValidation)
		void AddFightUIClient(const FString & StateID);

	UFUNCTION(BlueprintCallable)
	    void TogglePauseUI();

	UFUNCTION(BlueprintCallable)
		void TogglePlayerDeck();

	UFUNCTION(BlueprintCallable)
		void AddGameStartUI();

	UFUNCTION(BlueprintCallable)
		void RemoveGameStartUI();

	UFUNCTION(BlueprintCallable)
		void RemoveFightUI(const FString & StateID);

	UFUNCTION(Client, Reliable, WithValidation)
		void RemoveFightUIClient(const FString & StateID);

	UFUNCTION(BlueprintCallable)
		void SetPlayerStrength(int32 Value);
	UFUNCTION(Server, Reliable, WithValidation)
		void SetPlayerStrengthServer(int32 Value); //Used after rolling dice in fight mode

	UFUNCTION(BlueprintCallable)
		void ToggleChatWindow();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void UpdateWeaponMeshMulticast(int32 Index);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void UpdatePlayerMeshMulticast(int32 Index);

	virtual void EnterFightMode(AActor* Enemy) override;

	virtual void GameLost() override;

	virtual void GameWon() override;

	UFUNCTION(BlueprintPure)
	class UInvadedStateMachine* GetInvadedStateMachine() const { return InvadedStateMachine; }


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

	virtual void UseCard(class ACardBase* Card) override;



	virtual void ClearLastPath() override;

	UFUNCTION(BlueprintCallable)
	void LockCamera();

	UFUNCTION(BlueprintCallable)
	void UnlockCamera();

	void InitFightGame();

	UFUNCTION(Server, Reliable, WithValidation)
		void InitFightGameServer();
protected:

	UFUNCTION(BlueprintImplementableEvent)
		void BP_UseCard(ACardBase * Card);

	UFUNCTION(BlueprintImplementableEvent)
		void BP_ToggleHUD();

	UFUNCTION(BlueprintImplementableEvent)
		void BP_CollapseHUD();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void RemoveWidgets();

	UFUNCTION(Client, Reliable, WithValidation)
		void RemoveWidgetsClient();

	virtual void ClearLastRetracePath() override;

	UFUNCTION(Client, Reliable, WithValidation)
		void AddGameStartUIClient();

	UFUNCTION(Client, Reliable, WithValidation)
		void RemoveGameStartUIClient();
	
	

	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool GetStartAsSpectator() const { return bStartAsSpectator; }
	UPROPERTY(BlueprintReadWrite, Category = PlayerMovement, meta = (AllowPrivateAccess = "true"))
		EMoveStatus MoveStatus = EMoveStatus::MS_Idle;
	UFUNCTION(BlueprintCallable, Client, Reliable, WithValidation)
		void PlayFightAnim(AInvadedCharacter* Enemy);
	
	UFUNCTION(BlueprintCallable)
		void PlayReverseFightAnim();
protected:

	UPROPERTY(BlueprintReadWrite)
	class UMainGameWidget* MainGameWidget;

	UFUNCTION(Client, Reliable, WithValidation)
		void PlayReverseFightAnimClient();

		void SetPlayerColor(const FLinearColor& Value);
	UFUNCTION(Server, Reliable, WithValidation)
		void SetPlayerColorServer(const FLinearColor& Value);

		void SetFightCleaned(bool Value);
	UFUNCTION(Server, Reliable, WithValidation)
		void SetFightCleanedServer(bool Value);

	


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCineCameraComponent* TopDownCameraComponent;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	FTimerDelegate HighlightDelegate;
	FTimerHandle HighlightTimerHandle;
	FTimerHandle CellTimerHandle;
	FTimerHandle PlayerStateHandle;
	int bCheck = 1;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		bool bStartAsSpectator;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "UI")
	class UInvadedStateMachine* InvadedStateMachine;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "UI")
	class UUserWidget* PauseUI;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "UI")
		class UUserWidget* PlayerDeckUI;


	class UInvadedData* InvadedPS;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class UInvadedStateMachine> InvadedStateMachineClass;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class UUserWidget> PauseUIClass;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class UUserWidget> PlayerDeckUIClass;
	UPROPERTY(EditDefaultsOnly, Category = "Spectator")
		TSubclassOf<class UUserWidget> LoadingWidgetClass;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "UI")
	class UUserWidget* LoadingWidget;
	

	void AddLoadingWidget();

	UPROPERTY(EditDefaultsOnly, Category = "Zoom")
		float NewDesiredArmLength=600.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Zoom")
		float MinZoom;

	UPROPERTY(EditDefaultsOnly, Category = "Zoom")
		float MaxZoom;

	UPROPERTY(EditDefaultsOnly)
		float HighlightFrequency;


	void SetBoundaries();

	bool IsInsideBoundaries(FVector Value);

	FVector2D BoundariesX;
	FVector2D BoundariesY;
	FVector2D BoundariesZ;

	bool bLockCamera = false;

	
	FString CurrentFightStateID;
};
