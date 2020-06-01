// Fill out your copyright notice in the Description page of Project Settings.

#include "InvadedData.h"
#include "InvadedGM.h"
#include "UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "LobbySG.h"
#include "Engine/World.h"

UInvadedData::UInvadedData()
{
	PlayerMode = EPlayerMode::PM_Base;
	EnemyAction = EEnemyAction::EA_Base;
	MoveStatus = EMoveStatus::MS_Idle;
	bIsAlive = true;
	bReplicates = true;

}
void UInvadedData::SetIsPlayerMoving(const bool& Value)
{
	bMovePlayer = Value;
}
void UInvadedData::SetIsAIPlayer(const bool & Value)
{
	bIsAIPlayer = Value;
}
void UInvadedData::SetIsCurrentPlayer(const bool & Value)
{
	bCurrentPlayer = Value;
	if(!Value)
	SetHasTrownDice(false);
}
void UInvadedData::SetRemainingMoves(const uint8& Value)
{
	RemainingMoves = Value;

}
void UInvadedData::SetPlayerColor(FLinearColor Value)
{
	if (GetOwnerRole() < ROLE_Authority&&!GetOwner()->IsA<AInvadedAIPlayer>())
	{
		SetPlayerColorServer(Value);
	}
	else 
	{
		PlayerColor = Value;
	}
}
void UInvadedData::SetPlayerColorServer_Implementation(FLinearColor Value)
{
	SetPlayerColor(Value);
}
bool UInvadedData::SetPlayerColorServer_Validate(FLinearColor Value)
{
	return true;
}
void UInvadedData::SetFlagColor(FLinearColor  Value)
{
	if(GetOwnerRole()< ROLE_Authority && !GetOwner()->IsA<AInvadedAIPlayer>())
	{
		SetFlagColorServer(Value);
	}
	else
	{
		FlagColor = Value;
	}
	
}
void UInvadedData::SetFlagColorServer_Implementation(FLinearColor Value)
{
	SetFlagColor(Value);
}
bool UInvadedData::SetFlagColorServer_Validate(FLinearColor Value)
{
	return true;
}
void UInvadedData::LoadVariables(int32 PlayerId)
{	
}
void UInvadedData::SetPlayerMode(EPlayerMode Value)
{
	
	PlayerMode = Value;
}
void UInvadedData::SetEnemyAction(EEnemyAction Value)
{
	EnemyAction = Value;
}
void UInvadedData::SetMoveStatus(EMoveStatus Value)
{
	MoveStatus = Value;
}
void UInvadedData::SetPlayerName(FString Value)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		SetPlayerNameServer(Value);
	}
	else
	{
		PlayerName = Value;
	}
	
}
void UInvadedData::SetPlayerNameServer_Implementation(const FString& Value)
{
	SetPlayerName(Value);
}
bool UInvadedData::SetPlayerNameServer_Validate(const FString& Value)
{
	return true;
}
void UInvadedData::SetTurnScore(const int32 & Value)
{
	TurnScore = Value;
}
void UInvadedData::SetIsAlive(const bool & Value)
{
	bIsAlive = Value;
}
void UInvadedData::SetAIPlayerID(int32  Value)
{
	AIPlayerID = Value;
}
void UInvadedData::SetHasTrownDice(bool Value)
{
	bDiceThrown = Value;
}
void UInvadedData::SetOrder(int32 Value)
{
	Order = Value;
}
void UInvadedData::SetAdditionalThrow(bool Value)
{
	if (Value)
	{
		bAdditionalThrow++;
	}
	else
	{
		bAdditionalThrow = FMath::Clamp(bAdditionalThrow - 1, 0, bAdditionalThrow);
	}
	
}
void UInvadedData::AddRemainingMoves(const uint8& Value)
{
	RemainingMoves += Value;
}
bool UInvadedData::UserChatServer_Validate(const FSChatMessage& NewMessage)
{
	return true;
}
void UInvadedData::UserChatServer_Implementation(const FSChatMessage& NewMessage)
{
	UserChatMulticast(NewMessage);
}
bool UInvadedData::UserChatMulticast_Validate(const FSChatMessage& NewMessage)
{
	return true;
}
void UInvadedData::UserChatMulticast_Implementation(const FSChatMessage& NewMessage)
{
	APlayerController* PlayerCon;
	APlayerHUD* PlayerHUD;

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator) // find all controllers
	{
		PlayerCon = Cast<APlayerController>(*Iterator);
		if (PlayerCon)
		{
			PlayerHUD = Cast<APlayerHUD>(PlayerCon->GetHUD());
			if (PlayerHUD)
				PlayerHUD->AddMessageBP(NewMessage); // place the chat message on this player controller
		}
	}
}
void UInvadedData::SetIsAttacker(bool Value)
{
	bIsAttacker = Value;
}
void UInvadedData::SetWeaponIndex(int32 Value)
{
	WeaponIndex = Value;
}
void UInvadedData::SetMeshIndex(int32 Value)
{
	MeshIndex = Value;
}
void UInvadedData::SetFlagTextureIndex(int32 Value)
{
	FlagTextureIndex = Value;
}
void UInvadedData::SetLastRemainingMoves(int32 Value)
{
	LastRemainingMoves = Value;
}
void UInvadedData::SetSteamTexture(UTexture2D * Value)
{

}
void UInvadedData::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInvadedData, bCurrentPlayer);
	DOREPLIFETIME(UInvadedData, bIsAlive);
	DOREPLIFETIME(UInvadedData, RemainingMoves);
	DOREPLIFETIME(UInvadedData, bMovePlayer);
	DOREPLIFETIME(UInvadedData, PlayerColor);
	DOREPLIFETIME(UInvadedData, PlayerMode);
	DOREPLIFETIME(UInvadedData, EnemyAction);
	DOREPLIFETIME(UInvadedData, MoveStatus);
	DOREPLIFETIME(UInvadedData, FlagColor);
	DOREPLIFETIME(UInvadedData, TurnScore);
	DOREPLIFETIME(UInvadedData, bIsAIPlayer);
	DOREPLIFETIME(UInvadedData, AIPlayerID);
	DOREPLIFETIME(UInvadedData, PlayerName);
	DOREPLIFETIME(UInvadedData, bTest);
	DOREPLIFETIME(UInvadedData, bDiceThrown);
	DOREPLIFETIME(UInvadedData, Order);
	DOREPLIFETIME(UInvadedData, bAdditionalThrow);
	DOREPLIFETIME(UInvadedData, bIsAttacker);
	DOREPLIFETIME(UInvadedData, WeaponIndex);
	DOREPLIFETIME(UInvadedData, MeshIndex);
	DOREPLIFETIME(UInvadedData, FlagTextureIndex);
	DOREPLIFETIME(UInvadedData, LastRemainingMoves);
	
}