// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/FightInterface.h"
#include "Interfaces/HighlightInterface.h"
#include "GameModes/InvadedData.h"
#include "Actors/Flag.h"
#include "InvadedData.h"
#include "FightGameComponent.h"
#include "OnlineStats.h"
#include "InvadedCharacter.generated.h"

UCLASS()
class INVADED_API AInvadedCharacter : public ACharacter , public IFightInterface , public IHighlightInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	class UFightGameComponent* FightGameComponent;
public:

	UFUNCTION(BlueprintPure)
	class UFightGameComponent* GetFightGameComponent() const { return FightGameComponent; }

	/*Interfaces*/

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Fight")
		void FightWon(class AInvadedCharacter* Enemy);
	virtual void FightWon_Implementation(class AInvadedCharacter* Enemy);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Fight")
		void FightDraw(class AInvadedCharacter* Enemy);
	virtual void FightDraw_Implementation(class AInvadedCharacter* Enemy);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Fight")
		void FightLost(class AInvadedCharacter* Enemy);
	virtual void FightLost_Implementation(class AInvadedCharacter* Enemy);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Highlight")
		void OnBeginFocus();
	virtual void OnBeginFocus_Implementation();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Highlight")
		void OnEndFocus();
	virtual void  OnEndFocus_Implementation();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Highlight")
		void SetHighlightState(EHighlightState Value);
	virtual void SetHighlightState_Implementation(EHighlightState Value);

	// Sets default values for this character's properties
	AInvadedCharacter(const FObjectInitializer& ObjectInitializer);

	class ACellBase* GetLastFocusedCell() { return LastFocusedCell; }

	UFUNCTION(BlueprintPure)
	class ACellBase* GetOversteppedCell() { return OversteppedCell; }

	class AInvadedCharacter* GetLastFocusedPlayer() { return LastFocusedPlayer; }

	TArray<class ACellBase*> GetCurrentPath() { return LastPath; }



	UFUNCTION(BlueprintCallable, Category = "Player")
		virtual void SetPlayerMode(EPlayerMode Value);

	UFUNCTION(Server, Reliable, WithValidation)
		void SetPlayerModeServer(EPlayerMode Value);

	UFUNCTION(BlueprintPure)
	bool HasFlag()
	{
		if (Flags.Num()) return true;
		else return false;
	}

	UFUNCTION(BlueprintPure,Category="A Star")
	TArray<class ACellBase*> GetRPath() const { return RPath; }

	TArray<class AFlag*> GetFlags() const { return Flags; }

	void RemoveFlags();

	UFUNCTION(Server,Reliable,WithValidation)
	void RemoveFlagsServer();

	void AddFlag(class AFlag* Value);

	void AddFlags(const TArray<AFlag*>& Array);

	UFUNCTION(Server,Reliable,WithValidation)
	void AddFlagsServer(const TArray<AFlag*>& Array);

	void RetraceThePath();

	virtual void EnterFightMode(AActor* Enemy);

	void SetStartingCell(class ACellBase* Value);

	UFUNCTION(Server,Reliable,WithValidation)
	void SetStartingCellServer(class ACellBase* Value);
	

	UFUNCTION(BlueprintPure)
	class ACellBase* GetStartingCell() const { return StartingCell; }

	virtual void GameLost();
	virtual void GameWon();
	virtual void DropFlag();

	UFUNCTION(Server,Reliable,WithValidation)
	void DropFlagServer();

	virtual void ToggleDicePod();

	bool ProjectPath(int32 Distance,TArray<ACellBase*> CellPath);
	virtual void KilledPlayer();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void ChangeCellHighlight(class ACellBase* Cell, bool bSwitch, EHighlightState HighlightState);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void UpdateCellMaterialMulitcast(class ACellBase* Cell, const FLinearColor& BaseColor, EHighlightState HighlightState);

	void UpdateCellMaterial(class ACellBase* Cell, const FLinearColor& BaseColor, EHighlightState HighlightState);

	UFUNCTION(Server, Reliable, WithValidation)
		void UpdateCellMaterialServer(class ACellBase* Cell, const FLinearColor& BaseColor, EHighlightState HighlightState);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void ForceChangeCellHighlight(class ACellBase* Cell, bool bSwitch, EHighlightState HighlightState);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void ChangePlayerHighlightMulticast(class AInvadedCharacter* Player, bool bSwitch, EHighlightState HighlightState);

		void ChangePlayerHighlight(class AInvadedCharacter* Player, bool bSwitch, EHighlightState HighlightState);

	UFUNCTION(Server,Reliable,WithValidation)
		void ChangePlayerHighlightServer(class AInvadedCharacter* Player, bool bSwitch, EHighlightState HighlightState);

	UFUNCTION(BlueprintCallable)
	UInvadedData* GetInvadedPS() const { return InvadedPS; }

	UFUNCTION(BlueprintCallable)
		void TeleportPlayer(class AInvadedCharacter* Player, FVector Location);
	UFUNCTION(Server,Reliable,WithValidation)
		void TeleportPlayerServer(class AInvadedCharacter* Player, FVector Location);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void TeleportPlayerMulticast(class AInvadedCharacter* Player, FVector Location);

		void ThrowDice();
	UFUNCTION(Server, Reliable, WithValidation)
		void ThrowDiceServer();


	void SetLastFocusedCell(class ACellBase* Value);

	void SetOversteppedCell(class ACellBase* Value);

	void RemoveFlag(class AFlag* Value);

	UFUNCTION(BlueprintCallable)
		TArray<class ACardBase*> GetCardArray() const { return CardsArray; }

	void FindOversteppedCell();

	UFUNCTION(BlueprintCallable)
		void ClearSelections();

	UFUNCTION(BlueprintCallable)
	void SetPlayerFightValue(int32 Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void SetPlayerFightValueServer(int32 Value);

	virtual void AddCard(class ACardBase* Value);

	UFUNCTION(BlueprintCallable)
		virtual void UseCard(class ACardBase* Card);

	UFUNCTION(BlueprintCallable)
		class UInvadedMovementComp* GetInvadedMovementComp() const { return InvadedMovementComp; }
	UFUNCTION(BlueprintCallable)
		class UInvadedAnimInstance* GetAnimInstance() const { return AnimInstance; }

	void NextPathCell();

	virtual void CallTurnEnd();

	UFUNCTION(BlueprintCallable, Category = "GameMode")
		void ForceCallTurnEnd();

	UFUNCTION(Server,Reliable,WithValidation)
		void ForceCallTurnEndServer();

	void SetCurrentPath(const TArray<class ACellBase*>& Value);

	virtual void ClearLastPath();

	TArray<class ACellBase*> GetLastPath() const { return LastPath; }

	bool bFightCleaned = true;

	UPROPERTY(Replicated)
		int32 FightStatus;

	virtual void ClearLastRetracePath();

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
		class UStaticMeshComponent* SM_Rifle;

	class ACardBase* GetLastUsedCard() { return LastUsedCard; }

	/*Achievements*/

/** Get all the available achievements and cache them */
	void QueryAchievements();

	/** Called when the cache finishes */
	void OnQueryAchievementsComplete(const FUniqueNetId& PlayerId, const bool bWasSuccessful);

	/** Updates the achievement progress */
	UFUNCTION(BlueprintCallable, Category = "Achievements")
		void UpdateAchievementProgress(FString Id, float Percent);

	/** The object we're going to use in order to progress any achievement */
	FOnlineAchievementsWritePtr AchievementsWriteObjectPtr;

	/** Progress win game achievement */
	UFUNCTION(BlueprintCallable, Category = "Achievements")
		void UnlockAchievement(FString Id);
protected:

	UPROPERTY(EditAnywhere)
		class UMaterialParameterCollection* MaterialParameterCollectionAsset;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetPathFound(bool Value);

	UFUNCTION(Server, Reliable, WithValidation)
		void SetLastFocusedCellServer(class ACellBase* Value);
	void SetLastFocusedPlayer(class AInvadedCharacter* Value);
	UFUNCTION(Server, Reliable, WithValidation)
		void SetLastFocusedPlayerServer(class AInvadedCharacter* Value);


	UFUNCTION(Server, Reliable, WithValidation)
		void SetOversteppedCellServer(class ACellBase* Value);

	UFUNCTION(Server, Reliable, WithValidation)
		void CallTurnEndServer();

	UFUNCTION(Server, Reliable, WithValidation)
		void UseCardServer(class ACardBase* Card);


	UFUNCTION(Server, Reliable, WithValidation)
		void AddCardServer(class ACardBase* Value);

	/*Highlight*/
	virtual void HighlightCell();
	virtual void HighlightCornerCell();
	virtual void HighlightPlayer();

	virtual void NewCellPath(class ACellBase* Cell);
	UFUNCTION(Server, Reliable, WithValidation)
		void NewCellPathServer(class ACellBase* Cell);

	void DeoccupyCell(class ACellBase* Cell);
	UFUNCTION(Server, Reliable, WithValidation)
		virtual void DeoccupyCellServer(class ACellBase* Cell);

	void RotatePlayer(float Rotation);

	UFUNCTION(Server, Reliable, WithValidation)
		void RotatePlayerServer(float Rotation);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void RotatePlayerMulticast(float Rotation);


	UFUNCTION()
		virtual void FindPath(class ACellBase* StartCell,class ACellBase* EndCell);

	

	UFUNCTION(Server, Reliable, WithValidation)
		void RetraceThePathServer();

	UFUNCTION(Server, Reliable, WithValidation)
		void DestroyFlagServer(class AFlag* Value);

	    virtual void DestroyFlag(class AFlag* Value);


	UFUNCTION(Server, Reliable, WithValidation)
		void NextPathCellServer();
	
		

	UFUNCTION(Server, Reliable, WithValidation)
		void SetCurrentPathServer(const TArray<class ACellBase*>& Value);


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UInvadedData* InvadedPS;


	bool bPathFound = true;
	UPROPERTY(Replicated)
		TArray<class ACellBase*> Path;
	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		TArray<class ACellBase*> LastPath;
	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		TArray<class ACardBase*> CardsArray;

	class UInvadedMovementComp* InvadedMovementComp;


	UPROPERTY(Replicated)
		class ACellBase* LastFocusedCell;
	UPROPERTY(Replicated)
		class ACellBase* LastTargetCell;
	UPROPERTY(Replicated)
		class ACellBase* OversteppedCell;
	UPROPERTY(Replicated)
		class AInvadedCharacter* LastFocusedPlayer;
	UPROPERTY(Replicated)
		class ACardBase* LastUsedCard;

	class UInvadedAnimInstance* AnimInstance;

	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		TArray<class AFlag*> Flags;

	UPROPERTY(Replicated,BlueprintReadOnly)
		TArray<class ACellBase*> RPath;
	UPROPERTY(Replicated, BlueprintReadOnly)
		TArray<class ACellBase*> LastRetracePath;


	class ACellBase* StartingCell;

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void ClearLastRetracePathServer();

	UFUNCTION(Client, Reliable, WithValidation)
		void ClearSelectionsClient();
protected:



	UPROPERTY(EditDefaultsOnly)
		FLinearColor PositiveColor;

	UPROPERTY(EditDefaultsOnly)
		FLinearColor NegativeColor;

	
	class UMaterialInstanceDynamic* CharacterMatDyn;

	UPROPERTY(EditDefaultsOnly, Category = "Color")
		bool bSetColorVariables;
	UPROPERTY(EditDefaultsOnly, Category = "Color", meta = (EditCondition = "bSetColorVariables"))
		FLinearColor FlagColor;
	UPROPERTY(EditDefaultsOnly, Category = "Color", meta = (EditCondition = "bSetColorVariables"))
		FLinearColor PlayerColor;


	void HighlightPath();

	UFUNCTION(Server, Reliable, WithValidation)
		void HighlightPathServer();

	UPROPERTY(Replicated)
	bool bThrownDice;

	UPROPERTY(Replicated)
		bool bInitialized;

	UPROPERTY(EditDefaultsOnly, Category = "Slots")
		TArray<FName> FlagSockets;

	UPROPERTY(EditDefaultsOnly, Category = "Slots")
		FName DefaultFlagSocketName = "Flag";

	UPROPERTY(EditDefaultsOnly, Category = "Meshes")
		TArray<class UStaticMesh*> WeaponArray;
	UPROPERTY(EditDefaultsOnly, Category = "Meshes")
		TArray<class USkeletalMesh*> MeshArray;
	UPROPERTY(EditDefaultsOnly, Category = "Meshes")
		TArray<class UAnimBlueprintGeneratedClass*> AnimationBlueprintArray;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	
};
