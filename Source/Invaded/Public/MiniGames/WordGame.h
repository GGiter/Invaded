// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FightGame.h"
#include "TimerManager.h"
#include "WordGame.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API AWordGame : public AFightGame
{
	GENERATED_BODY()
public:
	AWordGame();

	virtual void BeginPlay() override;

	virtual void UpdateScore(class UInvadedData* InvadedPS, int32 Score) override;
	virtual void DetermineFightWinner() override;

	virtual void Init(UInvadedData * InvadedPS) override;

	virtual void EndGame(UInvadedData * InvadedPS) override;

	virtual void StartGame() override;

	UFUNCTION(BlueprintCallable)
	FString GetNextSequence(class UInvadedData* InvadedPS);

	virtual int32 GetEnemyRemainingTries(class UInvadedData* CallingPlayer) override;
protected:
	
	void SetupData();

	void GuessWord();

	TArray<UInvadedData*> AIPlayers;

	int32 FirstFighterHealth;

	int32 SecondFighterHealth;
	
	UPROPERTY(Replicated)
	TArray<FString> SequenceArray;

	int32 FirstFighterSeqIndex;

	int32 SecondFighterSeqIndex;

	FString GenerateWord();

	UPROPERTY(EditAnywhere)
		float GameTime;
	UPROPERTY(EditAnywhere)
		int32 NumberOfTries;

	UPROPERTY(EditAnywhere)
		TArray<FString> WordList;

	UPROPERTY(EditAnywhere)
		float AIFailRate;
	UPROPERTY(EditAnywhere)
		float AISpeed;

	FTimerHandle GuessWordTimer;
};
