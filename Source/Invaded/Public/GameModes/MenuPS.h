// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SaveGames/LobbySG.h"
#include "MenuPS.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API AMenuPS : public APlayerState
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable)
		void SetPlayerColor(FLinearColor Color);
	UFUNCTION(BlueprintCallable)
		void SetFlagColor(FLinearColor Value);
	UFUNCTION(BlueprintCallable)
		void SetWeaponIndex(int32 Value);
	UFUNCTION(BlueprintCallable)
		void SetMeshIndex(int32 Value);
	UFUNCTION(BlueprintCallable)
		void SetFlagTextureIndex(int32 Value);
	
	UFUNCTION(BlueprintPure)
		FLinearColor GetPlayerColor() const { return PlayerColor; }
	UFUNCTION(BlueprintPure)
		FLinearColor GetFlagColor() const { return FlagColor; }
	UFUNCTION(BlueprintPure)
		int32 GetWeaponIndex() const { return WeaponIndex; }
	UFUNCTION(BlueprintPure)
		int32 GetMeshIndex() const { return MeshIndex; }
	UFUNCTION(BlueprintPure)
		int32 GetFlagTextureIndex() const { return FlagTextureIndex; }
	UFUNCTION(BlueprintCallable)
		void SaveVariables();

	UFUNCTION(BlueprintCallable)
		virtual void SetPlayerName(const FString& S) override;
protected:
	UFUNCTION(Server, Reliable, WithValidation)
		void SetPlayerColorServer(FLinearColor Color);
	UFUNCTION(Server, Reliable, WithValidation)
		void SetFlagColorServer(FLinearColor Color);
	UFUNCTION(Server, Reliable, WithValidation)
		void SetWeaponIndexServer(int32 Value);
	UFUNCTION(Server, Reliable, WithValidation)
		void SetMeshIndexServer(int32 Value);
	UFUNCTION(Server, Reliable, WithValidation)
		void SetFlagTextureIndexServer(const int32& Value);

	UFUNCTION(Server, Reliable, WithValidation)
		void SetPlayerNameServer(const FString& S);
private:

	UPROPERTY(Replicated)
		FLinearColor PlayerColor = { 0.0f,0.0f,0.0f,0.0f };
	UPROPERTY(Replicated)
		FLinearColor FlagColor = { 0.0f,0.0f,0.0f,0.0f };
	UPROPERTY(Replicated)
		int32 WeaponIndex = 0;
	UPROPERTY(Replicated)
		int32 MeshIndex = 0;
	UPROPERTY(Replicated)
		int32 FlagTextureIndex=0;


	
	
};
