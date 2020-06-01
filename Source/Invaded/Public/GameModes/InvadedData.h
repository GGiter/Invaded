// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InvadedGS.h"
#include "InvadedAnimInstance.h"
#include "Player/PlayerHUD.h"
#include "InvadedData.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EPlayerMode : uint8 {
	PM_Base UMETA(DisplayName = "Base"),
	PM_Fight UMETA(DisplayName = "Fight"),
	PM_CornerCellSelection UMETA(DisplayName = "CornerCellSelection"),
	PM_PlayerSelection UMETA(DisplayName = "PawnSelection"),
	PM_UI UMETA(DisplayName ="UI")
};


UCLASS()
class INVADED_API UInvadedData : public UActorComponent
{
	GENERATED_BODY()
public:
	UInvadedData();
	UFUNCTION(BlueprintPure)
		bool GetIsAIPlayer() const { return bIsAIPlayer; }
	UFUNCTION(BlueprintCallable)
		void SetIsAIPlayer(const bool& Value);
	UFUNCTION(BlueprintPure)
		bool GetIsCurrentPlayer() const { return bCurrentPlayer; }
	UFUNCTION(BlueprintCallable)
		void SetIsCurrentPlayer(const bool& Value);
	UFUNCTION(BlueprintCallable)
		void SetIsPlayerMoving(const bool& Value);
	UFUNCTION(BlueprintPure)
		bool IsPlayerMoving() const { return bMovePlayer; }
	UFUNCTION(BlueprintPure)
		uint8 GetRemainingMoves() const { return RemainingMoves; }
	UFUNCTION(BlueprintCallable)
		void SetRemainingMoves(const uint8& Value);
	UFUNCTION(BlueprintCallable)
		void AddRemainingMoves(const uint8& Value);
	UFUNCTION(BlueprintCallable)
		void SetPlayerColor(FLinearColor Value);
	UFUNCTION(Server, Reliable, WithValidation)
		void SetPlayerColorServer(FLinearColor Value);
	UFUNCTION(BlueprintPure)
		FLinearColor GetPlayerColor() const { return PlayerColor; }
	UFUNCTION(BlueprintCallable)
		void SetFlagColor(FLinearColor Value);
	UFUNCTION(Server, Reliable, WithValidation)
		void SetFlagColorServer(FLinearColor Value);
	UFUNCTION(BlueprintPure)
		FLinearColor GetFlagColor() const { return FlagColor; }
	UFUNCTION(BlueprintCallable)
		void LoadVariables(int32 PlayerId);
	UFUNCTION(BlueprintCallable)
		void SetPlayerMode(EPlayerMode Value);
	UFUNCTION(BlueprintPure)
		EPlayerMode GetPlayerMode() const { return PlayerMode; }
	UFUNCTION(BlueprintCallable)
		void SetEnemyAction(EEnemyAction Value);
	UFUNCTION(BlueprintPure)
		EEnemyAction GetEnemyAction() const { return EnemyAction; }
	UFUNCTION(BlueprintCallable)
		void SetMoveStatus(EMoveStatus Value);
	UFUNCTION(BlueprintPure)
		EMoveStatus GetMoveStatus() const { return MoveStatus; }
	UFUNCTION(BlueprintCallable)
		void SetPlayerName(FString Value);
	UFUNCTION(Server, Reliable, WithValidation)
		void SetPlayerNameServer(const FString& Value);
	UFUNCTION(BlueprintPure)
		FString GetPlayerName() const { return PlayerName; }

	UFUNCTION(BlueprintCallable)
		void SetTurnScore(const int32& Value);

	UFUNCTION(BlueprintPure)
		int32 GetTurnScore() const { return TurnScore; }

	UFUNCTION(BlueprintCallable)
		void SetIsAlive(const bool& Value);
	UFUNCTION(BlueprintPure)
		bool GetIsAlive() const { return bIsAlive; }
	UFUNCTION(BlueprintCallable)
		void SetAIPlayerID(int32 Value);
	UFUNCTION(BlueprintPure)
		int32 GetAIPlayerID() const { return AIPlayerID; }

	UFUNCTION(Server, Reliable, WithValidation)
		virtual void UserChatServer(const FSChatMessage& NewMessage);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		virtual void UserChatMulticast(const FSChatMessage& NewMessage);

	UFUNCTION(BlueprintCallable)
		void SetHasTrownDice(bool Value);

	UFUNCTION(BlueprintPure)
		bool HasThrownDice() const { return bDiceThrown; }

	UFUNCTION(BlueprintCallable)
		void SetOrder(int32 Value);

	UFUNCTION(BlueprintPure)
		int32 GetOrder() const { return Order; }

	UFUNCTION(BlueprintCallable)
		void SetAdditionalThrow(bool Value);

	UFUNCTION(BlueprintPure)
		bool GetAdditionalThrow() const { return bAdditionalThrow; }

	UFUNCTION(BlueprintCallable)
		void SetIsAttacker(bool Value);

	UFUNCTION(BlueprintPure)
		bool GetIsAttacker() const { return bIsAttacker; }
	
	UFUNCTION(BlueprintCallable)
		void SetWeaponIndex(int32 Value);

	UFUNCTION(BlueprintPure)
		int32 GetWeaponIndex() const { return WeaponIndex; }

	UFUNCTION(BlueprintCallable)
		void SetMeshIndex(int32 Value);

	UFUNCTION(BlueprintPure)
		int32 GetMeshIndex() const { return MeshIndex; }

	UFUNCTION(BlueprintCallable)
		void SetFlagTextureIndex(int32 Value);

	UFUNCTION(BlueprintPure)
		int32 GetFlagTextureIndex() const { return FlagTextureIndex; }

	UFUNCTION(BlueprintCallable)
		void SetLastRemainingMoves(int32 Value);

	UFUNCTION(BlueprintPure)
		int32 GetLastRemainingMoves() const { return LastRemainingMoves; }

	UFUNCTION(BlueprintCallable)
		void SetSteamTexture(UTexture2D* Value);

	UFUNCTION(BlueprintPure)
		UTexture2D* GetSteamTexture() const { return nullptr; }
private:
	UPROPERTY(Replicated)
	bool bCurrentPlayer = false;
	UPROPERTY(Replicated)
	uint8 RemainingMoves = 0;
	UPROPERTY(Replicated)
	bool bMovePlayer = false;
	UPROPERTY(Replicated)
	bool bDiceThrown =false;
	UPROPERTY(Replicated)
	FLinearColor PlayerColor = { 18,41,26,255 };
	UPROPERTY(Replicated)
	FLinearColor FlagColor = { 18,41,26,255 };
	UPROPERTY(Replicated)
	EPlayerMode PlayerMode=EPlayerMode::PM_Base;
	UPROPERTY(Replicated)
	EEnemyAction EnemyAction = EEnemyAction::EA_Base;
	UPROPERTY(Replicated)
	EMoveStatus MoveStatus;
	UPROPERTY(Replicated)
	int32 TurnScore;
	UPROPERTY(Replicated)
	bool bIsAlive;
	UPROPERTY(Replicated)
	bool bIsAIPlayer;
	UPROPERTY(Replicated)
	int32 AIPlayerID;
	UPROPERTY(Replicated)
	FString PlayerName;
	UPROPERTY(Replicated)
	bool bTest;
	UPROPERTY(Replicated)
	int32 Order;
	UPROPERTY(Replicated)
	int32 bAdditionalThrow;
	UPROPERTY(Replicated)
	bool bIsAttacker;
	UPROPERTY(Replicated)
	int32 WeaponIndex;
	UPROPERTY(Replicated)
	int32 MeshIndex;
	UPROPERTY(Replicated)
	int32 FlagTextureIndex=0;
	UPROPERTY(Replicated)
	int32 LastRemainingMoves;

};
