// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FightGame.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API AFightGame : public AActor
{
	GENERATED_BODY()
public:
	AFightGame();
	UFUNCTION(BlueprintCallable)
		virtual void Init(class UInvadedData* InvadedPS);
	UFUNCTION(BlueprintCallable)
		virtual void InitReached(class UInvadedData* InvadedPS);
	UFUNCTION(BlueprintCallable)
		virtual void UpdateScore(class UInvadedData* InvadedPS,int32 Score);
	UFUNCTION(BlueprintCallable)
		virtual void DetermineFightWinner();
	UFUNCTION(BlueprintCallable)
		virtual void StartGame();
	UFUNCTION(BlueprintCallable)
		virtual void EndGame(class UInvadedData* InvadedPS);
	UFUNCTION(BlueprintCallable)
		virtual void EndGameReached(class UInvadedData* InvadedPS);

	UFUNCTION(BlueprintCallable)
		void SetGameResults(class UInvadedData* WinnerValue, class UInvadedData* LoserValue,bool bIsAttackWinner);

	UFUNCTION(BlueprintPure)
		class UInvadedData* GetWinner() { return Winner; }
	UFUNCTION(BlueprintPure)
		class UInvadedData* GetLoser() { return Loser; }

	UFUNCTION(BlueprintPure)
		class UInvadedData* GetFirstFighter() { return FirstFighter; }
	UFUNCTION(BlueprintPure)
		class UInvadedData* GetSecondFighter() { return SecondFighter; }
	
	UFUNCTION(BlueprintPure)
		class UInvadedData* GetEnemy(class UInvadedData* CallingPlayer);

	UFUNCTION(BlueprintPure)
		int32 GetEnemyScore(class UInvadedData* CallingPlayer);

	UFUNCTION(BlueprintPure)
		virtual int32 GetEnemyRemainingTries(class UInvadedData* CallingPlayer);

	UFUNCTION(BlueprintPure)
		FString GetEnemyName(class UInvadedData* CallingPlayer);

	UFUNCTION(BlueprintPure)
		int32 GetPlayerScore(class UInvadedData* CallingPlayer);
	
	UFUNCTION(BlueprintPure)
		bool IsFirstFighterWinner() { return Winner == FirstFighter; }
	UFUNCTION(BlueprintPure)
		bool IsDraw() { return Winner == Loser; }
	
	UFUNCTION(BlueprintPure)
		FString GetWidgetStateID() const { return WidgetStateID; }


protected:

	bool CompareFighters(class UInvadedData* Fighter1, class UInvadedData* Fighter2);

	void UpdatePlayersData();

	UPROPERTY(EditAnywhere)
		FString WidgetStateID;

	UPROPERTY(Replicated)
	class UInvadedData* FirstFighter;

	UPROPERTY(Replicated)
	class UInvadedData* SecondFighter;
	UPROPERTY(Replicated)
	int32 FirstFighterScore = 0;
	UPROPERTY(Replicated)
	int32 SecondFighterScore = 0;

	bool bFirstPlayer;

	bool bSecondPlayer;

	UPROPERTY(Replicated)
	class UInvadedData* Winner;
	UPROPERTY(Replicated)
	class UInvadedData* Loser;

	UPROPERTY(Replicated)
	bool bGameReady;



};
