// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyGS.h"
#include "UnrealNetwork.h"
#include "LobbyGM.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"


ALobbyGS::ALobbyGS()
{
	MapInfo.MapName = "Garden";
	MapInfo.MapScale = 0.5f;
	MapInfo.GridTypeName = "Default";

	BattleType = EBattleType::Both;
}
bool ALobbyGS::GetIsPlayerReady(class APlayerState* PlayerState)
{
	if (PlayerReadyMapString.Find(PlayerState) == INDEX_NONE)
		return false;
	else
		return PlayerReadyMapBool[PlayerReadyMapString.Find(PlayerState)];	
}

int32 ALobbyGS::GetNumberOfReadyPlayers()
{	
	return NumOfReadyPlayers;
}
void ALobbyGS::SetStartWithFlags(bool Value)
{
	bStartWithFlags = Value;
}
void ALobbyGS::SetStartWithCards(bool Value)
{
	bStartWithCards = Value;
}
void ALobbyGS::SetStartWithFight(bool Value)
{
	bStartWithFight = Value;
}
void ALobbyGS::SetUseAllFightingTypes(bool Value)
{
	bUseAllFightingTypes = Value;
}
void ALobbyGS::SetMapInfo(FMapInfo Value)
{
	MapInfo = Value;
}
void ALobbyGS::SetBattleType(EBattleType Value)
{
	BattleType = Value;
}
void ALobbyGS::SetInstalledItemIDs(const TArray<int32>& Value)
{
	InstalledItemIDs = Value;
}
void ALobbyGS::LockPlayers()
{	
	ALobbyGM* LobbyGM=Cast<ALobbyGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (LobbyGM)
	{
		LobbyGM->LockPlayers();
	}
}
void ALobbyGS::SetIsPlayerReady(class APlayerState* PlayerState, const bool& Value)
{
	if (PlayerReadyMapString.Find(PlayerState) == INDEX_NONE)
	{
		PlayerReadyMapString.Add(PlayerState);
		PlayerReadyMapBool.Add(Value);
	}	
	else
	{
		PlayerReadyMapBool[PlayerReadyMapString.Find(PlayerState)] = Value;
	}
		


	NumOfReadyPlayers = 0;
	for (bool Bool : PlayerReadyMapBool)
	{
		if (Bool)
			NumOfReadyPlayers++;
	}

	if (GetNumberOfReadyPlayers() == PlayerArray.Num())
		LockPlayers();

	
}
void ALobbyGS::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyGS, PlayerReadyMapString);
	DOREPLIFETIME(ALobbyGS, PlayerReadyMapBool);
	DOREPLIFETIME(ALobbyGS, NumOfReadyPlayers);
	DOREPLIFETIME(ALobbyGS, bStartWithFlags);
	DOREPLIFETIME(ALobbyGS, bStartWithCards);
	DOREPLIFETIME(ALobbyGS, bStartWithFight);
	DOREPLIFETIME(ALobbyGS, bUseAllFightingTypes);
	DOREPLIFETIME(ALobbyGS, MapInfo);
	DOREPLIFETIME(ALobbyGS, BattleType);
	DOREPLIFETIME(ALobbyGS, InstalledItemIDs);
}