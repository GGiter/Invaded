// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "InvadedData.h"
#include "FightGame.h"
#include "InvadedGM.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API AInvadedGM : public AGameMode
{
	GENERATED_BODY()

public:
	UFUNCTION()
	class AGridGenerator* GetGridGenerator() const { return GridGenerator; }

	UFUNCTION()
	class ADicePod* GetDicePod() const { return DicePod; }

	UFUNCTION(BlueprintCallable)
	class AFightGame* GetCurrentFightGame() const { return CurrentFightGame; }

	UFUNCTION()
	void DiceHaveLanded(AActor* Dice, uint8 Value);

	UFUNCTION()
	void OnEndTurn();

	UFUNCTION()
	void ConfigurateFlags(TArray<class AFlag*> FlagArray);

	UFUNCTION()
	void DestroyFlag(class AFlag* Flag,class AInvadedCharacter* Player,class AInvadedCharacter* Causer);

	UFUNCTION()
	void ChangePlayerCamera();

	UFUNCTION()
	void InitPlayerLocation();

	UFUNCTION()
	void SetPlayerFightValue(class AInvadedCharacter* Fighter, uint8 Value);

	UFUNCTION()
	void DetermineFightWinner();

	UFUNCTION()
	void FinishDetermineFightWinner();

	UFUNCTION()
	void SetPlayerMode(class AInvadedCharacter* Player, EPlayerMode PlayerMode);

	UFUNCTION()
	TSubclassOf<class ACardBase> DrawAttackCard();

	UFUNCTION()
	TSubclassOf<class ACardBase> DrawDefensiveCard();

	UFUNCTION(BlueprintCallable)
	void ThrowDice();

	UFUNCTION(BlueprintCallable)
	void ThrowDiceSpecified(class UInvadedData* InvadedData);

	UFUNCTION()
	void AdditionalThrow();

	UFUNCTION()
	void ResetPlayerLocations();

	UFUNCTION()
	void ResetFlags();

	UFUNCTION()
	void AddNextBattleWinner(class AInvadedCharacter* Player);

	UFUNCTION()
	void AddNextBattleLoser(class AInvadedCharacter* Player);

	UFUNCTION()
	void SkipPlayerTurn(class AInvadedCharacter* Player);

	UFUNCTION()
	void ToggleDicePod();

	void SetStartWithFlags(bool Value);

	void SetStartWithCards(bool Value);

	void SetStartWithFight(bool Value);

	void SetUseAllFightingTypes(bool Value);

	void SetMapScale(float Value);

	float GetMapScale() const { return MapScale; }

	void FirstFight();

	bool StartFight(class UInvadedData* Fighter1 , class UInvadedData* Fighter2);
protected:
	
	void SetupFirstFight(TArray<class UInvadedData*> InvadedDataArray);

	FString GenerateActorName();
	virtual void BeginPlay() override;
	virtual void InitGameState() override;
	virtual void PostLogin(APlayerController * NewPlayer) override;
	void SetNextCurrentPlayer(const uint8& StartingIndex);

	AFightGame* CurrentFightGame;

	UPROPERTY(EditAnywhere)
	TArray< TSubclassOf<AFightGame> > FightGameTypes;
	
	TArray<class UInvadedData*> FirstFightArray;

	TArray<TPair<int32, int32> > OrderArray;

	int32 FirstFightCounter = 0;

	UFUNCTION()
	void EndGame();

	UFUNCTION()
	void NextTurn();
	
	UFUNCTION()
	void SetupUI();
	
	UFUNCTION()
	void SetPlayerStart();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateLeaderboardPlayerData(class UInvadedData* InvadedData);

	
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Grid")
		TSubclassOf<class AGridGenerator> GridGeneratorClass;
	UPROPERTY(EditDefaultsOnly, Category = "Cards")
		TArray<TSubclassOf<class ACardBase> > AttackCardsClasses;
	UPROPERTY(EditDefaultsOnly, Category = "Cards")
		TArray<TSubclassOf<class ACardBase> > DefensiveCardsClasses;
	UPROPERTY(EditDefaultsOnly, Category = "Cards")
		bool bStartWithCards = false;
	UPROPERTY(EditDefaultsOnly, Category = "Flags")
		bool bStartWithFlag = false;
	UPROPERTY(EditDefaultsOnly, Category = "Fight")
		bool bStartWithFight = false;
	UPROPERTY(EditDefaultsOnly, Category = "Fight")
		bool bRandomMiniGames = false;
	UPROPERTY(EditDefaultsOnly, Category = "Fight")
		bool bUseAllFightingTypes = true;
	UPROPERTY(EditDefaultsOnly, Category = "Fight")
		int32 MiniGameIndex = 0;
	UPROPERTY(EditDefaultsOnly, Category = "AI")
		TSubclassOf<class AInvadedAIPlayer> InvadedAIPlayerClass;
	void AddCardsToPlayer();
	void AddFlagsToPlayers(TArray<class AFlag*> FlagArray);
	class AGridGenerator* GridGenerator;
	class AInvadedGS* InvadedGS;
	class ADicePod* DicePod;
	TArray<class AFlag*> Flags;
	FTimerHandle ThrowDiceHandle;
	FTimerHandle PlayerLocationHandle;
	UPROPERTY(EditDefaultsOnly, Category = "Editor")
		bool bIsInEditor;
	UPROPERTY(EditDefaultsOnly, Category = "Editor")
		uint32 NumOfPlayers = 1;
	UPROPERTY(EditDefaultsOnly, Category = "Editor")
		int32 MaxNumOfPlayers = 2;
	UPROPERTY(EditDefaultsOnly, Category = "Editor")
		float MapScale = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Turn")
		float MaxTurnTime;

	UPROPERTY(EditDefaultsOnly, Category = "Delays")
		float InitPlayerLocationDelay = 3.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Delays")
		float ChangeCameraDelay = 2.0f;


	TArray<FString> AINames;

	bool bSwitchPlayers;

	bool bIsDraw;

	class AActor* Fighter1Actor;
	class AActor* Fighter2Actor;
	uint8 FighterStrenght1, FighterStrenght2;
	TArray<class APawn*> BattleWinners;
	TArray<class APawn*> BattleLosers;
	TArray<class UInvadedData*> SkipTurnPlayers;
	TArray<class AInvadedAIPlayer*> AIPlayers;
	TArray<class AInvadedCharacter*> Players;
	TArray<class ASpectatorPlayer*> Spectators;
	TArray<class UInvadedData*> PlayerStates;

	int32 bAdditionalThrow = 0;
	bool GameEnded;
	bool bPlayersReady = false;
	FTimerHandle testHandle;
	FTimerHandle EndGameHandle;
};
