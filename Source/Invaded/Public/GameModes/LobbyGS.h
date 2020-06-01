// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "SaveGames/LobbySG.h"
#include "LobbyGS.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API ALobbyGS : public AGameState
{
	GENERATED_BODY()
public:
	ALobbyGS();
	UFUNCTION(BlueprintCallable)
	void SetIsPlayerReady(class APlayerState* PlayerState, const bool& Value);
	UFUNCTION(BlueprintPure)
	bool GetIsPlayerReady(class APlayerState* PlayerState);
	UFUNCTION(BlueprintPure)
	int32 GetNumberOfReadyPlayers();
	UFUNCTION(BlueprintPure)
	FMapInfo GetMapInfo() const  { return MapInfo; }

	UFUNCTION(BlueprintPure)
		EBattleType GetBattleType() const { return BattleType; }

	UFUNCTION(BlueprintCallable)
		void SetStartWithFlags(bool Value);

	UFUNCTION(BlueprintCallable)
		void SetStartWithCards(bool Value);

	UFUNCTION(BlueprintCallable)
		void SetStartWithFight(bool Value);

	UFUNCTION(BlueprintCallable)
		void SetUseAllFightingTypes(bool Value);

	UFUNCTION(BlueprintCallable)
		void SetMapInfo(FMapInfo Value);

	UFUNCTION(BlueprintCallable)
		void SetBattleType(EBattleType Value);

	void SetInstalledItemIDs(const TArray<int32>& Value);

	UFUNCTION(BlueprintPure)
		bool GetStartWithCards() const { return bStartWithCards; }
	UFUNCTION(BlueprintPure)
		bool GetStartWithFlags() const { return bStartWithFlags; }
	UFUNCTION(BlueprintPure)
		bool GetStartWithFight() const { return bStartWithFight; }
	UFUNCTION(BlueprintPure)
		bool GetUseAllFightingTypes() const { return bUseAllFightingTypes; }
	UFUNCTION(BlueprintPure)
		TArray<int32> GetInstalledItemIDs() const { return InstalledItemIDs; }

private:
	void LockPlayers();
	UPROPERTY(Replicated)
		TArray<class APlayerState*> PlayerReadyMapString;
	UPROPERTY(Replicated)
		TArray<bool> PlayerReadyMapBool;
	UPROPERTY(Replicated)
		int32 NumOfReadyPlayers=0;
	UPROPERTY(Replicated)
		bool bStartWithFlags=false;
	UPROPERTY(Replicated)
		bool bStartWithCards = false;
	UPROPERTY(Replicated)
		bool bStartWithFight = false;
	UPROPERTY(Replicated)
		bool bUseAllFightingTypes = true;
	UPROPERTY(Replicated)
		FMapInfo MapInfo;
	UPROPERTY(Replicated)
		EBattleType BattleType;
	UPROPERTY(Replicated)
		TArray<int32> InstalledItemIDs;
	
};
