// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MiniGames/FightGame.h"
#include "ClickGame.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API AClickGame : public AFightGame
{
	GENERATED_BODY()
public:
	AClickGame();
	virtual void UpdateScore(class UInvadedData* InvadedPS, int32 Score) override;
	virtual void DetermineFightWinner() override;

	virtual void Init(UInvadedData * InvadedPS) override;

	virtual void EndGame(UInvadedData * InvadedPS) override;

	virtual void StartGame() override;

	int32 GetNextTarget(UInvadedData * InvadedPS);
private:

	void ClickPart();

	int32 GenerateNextTarget();

	TArray<int32> TargetArray;

	TArray<UInvadedData*> AIPlayers;

	int32 FirstFighterHealth;

	int32 SecondFighterHealth;


	int32 FirstFighterSeqIndex;

	int32 SecondFighterSeqIndex;

	UPROPERTY(EditAnywhere)
		float GameTime;
	UPROPERTY(EditAnywhere)
		int32 NumberOfTries;
	UPROPERTY(EditAnywhere)
		int32 MaxTarget;

	UPROPERTY(EditAnywhere)
		float AIFailRate;
	UPROPERTY(EditAnywhere)
		float AISpeed;

	FTimerHandle ClickPartTimer;
};
