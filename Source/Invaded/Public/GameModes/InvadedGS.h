// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SaveGames/LobbySG.h"
#include "InvadedGS.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EEnemyAction : uint8 {
	EA_Base UMETA(DisplayName = "Base"),
	EA_LoseTurn UMETA(DisplayName= "LoseTurn"),
	EA_LoseBattle UMETA(DisplayName = "LoseBattle"),
	EA_MoveToCornerCell UMETA(DisplayName = "MoveToCornerCell"),
	EA_Move UMETA(DisplayName = "Move")
};

UCLASS()
class INVADED_API AInvadedGS : public AGameState
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure)
	class UInvadedData* GetCurrentPlayer() 
	{ 
		return CurrentPlayerState;
	}
	UFUNCTION(BlueprintCallable)
	void ClearPlayerStates()
	{
		InvadedPlayerStates.Empty();
	}
	UFUNCTION(BlueprintCallable)
		void SetCurrentPlayer(class UInvadedData* PlayerState);
	UFUNCTION(BlueprintPure)
		uint8 GetCurrentPlayerIndex() const { return CurrentPlayerIndex; }
	UFUNCTION(BlueprintCallable)
		void SetCurrentPlayerIndex(const uint8& Value);
	UFUNCTION(BlueprintPure)
		uint8 GetCurrentTurn() const { return CurrentTurn; }
	UFUNCTION(BlueprintCallable)
		void SetCurrentTurn(const uint8& Value);
	UFUNCTION(BlueprintPure)
		class AGridGenerator* GetGridGenerator() const { return GridGenerator; }
	UFUNCTION(BlueprintCallable)
		void SetGridGenerator(class AGridGenerator* Value);
	UFUNCTION(BlueprintCallable)
		void LoadVariables(int32 PlayerID);
	UFUNCTION(BlueprintCallable)
		void SetNumberOfPlayers(int32 Value);
	UFUNCTION(BlueprintPure)
		int32 GetNumberOfPlayers() const { return NumOfPlayers; }
	UFUNCTION(BlueprintPure)
		int32 GetCurrentNumOfPlayers() const { return CurrentNumOfPlayers; }
	UFUNCTION(BlueprintCallable)
		void SetCurrentNumOfPlayers(int32 Value);
	UFUNCTION(BlueprintPure)
		int32 GetNumberOfAlivePlayers() const { return NumberOfAlivePlayers; }
	UFUNCTION(BlueprintCallable)
		void SetNumberOfAlivePlayers(int32 Value);
	UFUNCTION(BlueprintCallable)
		void AddInvadedPlayerState(class UInvadedData* Value);
	UFUNCTION(BlueprintCallable)
		void RemoveInvadedPlayerState(class UInvadedData* Value);
	UFUNCTION(BlueprintPure)
		TArray<UInvadedData*> GetPlayerStates() const { return InvadedPlayerStates; }
	UFUNCTION(BlueprintCallable)
		void SetPlayerStates(TArray<class UInvadedData*> Value);
	UFUNCTION(BlueprintCallable)
		void SetMapScale(float Value);
	UFUNCTION(BlueprintPure)
		float GetMapScale() { return MapScale; }
	UFUNCTION(BlueprintCallable)
		void SetMaxTurnTime(float Value);
	UFUNCTION(BlueprintPure)
		float GetTimeRemaining() { return TimeRemaining; }
	UFUNCTION(BlueprintPure)
		EBattleType GetBattleType() const { return BattleType; }
	
	UFUNCTION(BlueprintPure)
		FMapInfo GetMapInfo() { return MapInfo; }
private:

	void UpdateTime();

	UPROPERTY(Replicated)
	class UInvadedData* CurrentPlayerState;
	UPROPERTY(Replicated)
	uint8 CurrentPlayerIndex = 0;
	UPROPERTY(Replicated)
	uint8 CurrentTurn = 0;
	UPROPERTY(Replicated)
	class AGridGenerator* GridGenerator;
	UPROPERTY(Replicated)
	uint32 NumOfPlayers=2;
	UPROPERTY(Replicated)
	uint32 CurrentNumOfPlayers = 0;
	UPROPERTY(Replicated)
	int32 NumberOfAlivePlayers = 0;
	UPROPERTY(Replicated)
	TArray<UInvadedData*> InvadedPlayerStates;
	UPROPERTY(Replicated)
	float MapScale = 1.0f;
	UPROPERTY(Replicated)
	float TimeRemaining;
	UPROPERTY(Replicated)
	float MaxTurnTime;
	UPROPERTY(Replicated)
	FMapInfo MapInfo;
	UPROPERTY(Replicated)
	EBattleType BattleType;
	UPROPERTY(Replicated)
	FTimerHandle TimeUpdateHandle;
	
};
