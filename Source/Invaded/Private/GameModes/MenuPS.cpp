// Fill out your copyright notice in the Description page of Project Settings.

#include "MenuPS.h"
#include "UnrealNetwork.h"
#include "LobbySG.h"
#include "LobbyGS.h"
#include "Kismet/GameplayStatics.h"
#include "InvadedGS.h"
#include "Engine/World.h"
#include "PlayerSettings.h"


void AMenuPS::BeginPlay()
{
	Super::BeginPlay();
	UPlayerSettings::LoadSettings();
	SetPlayerColor(UPlayerSettings::GetColor1());
	SetFlagColor(UPlayerSettings::GetColor2());

}
void AMenuPS::SetPlayerColor(FLinearColor Color)
{
	if (Role < ROLE_Authority)
	{
		SetPlayerColorServer(Color);
	}
	else
	{
		PlayerColor = Color;
	}
}

void AMenuPS::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMenuPS, PlayerColor);
	DOREPLIFETIME(AMenuPS, FlagColor);
	DOREPLIFETIME(AMenuPS, WeaponIndex);
	DOREPLIFETIME(AMenuPS, MeshIndex);
	DOREPLIFETIME(AMenuPS, FlagTextureIndex);
}
void AMenuPS::SetFlagColor(FLinearColor  Value)
{
	if (Role < ROLE_Authority)
	{
		SetFlagColorServer(Value);
	}
	else
	{
		FlagColor = Value;
	}
	
}
void AMenuPS::SetWeaponIndex(int32  Value)
{
	if (Role < ROLE_Authority)
	{
		SetWeaponIndexServer(Value);
	}
	else
	{
		WeaponIndex = Value;
	}

}
void AMenuPS::SetMeshIndex(int32 Value)
{
	if (Role < ROLE_Authority)
	{
		SetMeshIndexServer(Value);
	}
	else
	{
		MeshIndex = Value;
	}
}
void AMenuPS::SetFlagTextureIndex(int32 Value)
{
	if (Role < ROLE_Authority)
	{
		SetFlagTextureIndexServer(Value);
	}
	else
	{
		FlagTextureIndex = Value;
	}
}
void AMenuPS::SetMeshIndexServer_Implementation(int32 Value)
{
	SetMeshIndex(Value);
}
bool AMenuPS::SetMeshIndexServer_Validate(int32 Value)
{
	return true;
}
void AMenuPS::SetFlagTextureIndexServer_Implementation(const int32& Value)
{
	SetFlagTextureIndex(Value);
}
bool AMenuPS::SetFlagTextureIndexServer_Validate(const int32& Value)
{
	return true;
}
void AMenuPS::SaveVariables()
{
	ULobbySG*SaveGameInstance = Cast<ULobbySG>(UGameplayStatics::CreateSaveGameObject(ULobbySG::StaticClass()));
	if (GetPlayerName() != "")
	{
		SaveGameInstance->PlayerName = GetPlayerName();
		SaveGameInstance->UserIndex = PlayerId;
		UE_LOG(LogTemp, Warning, TEXT("Saving Color %d %s"), PlayerId, *GetPlayerColor().ToString());
		SaveGameInstance->PlayerColor = GetPlayerColor();
		SaveGameInstance->FlagColor = GetFlagColor();
		SaveGameInstance->WeaponIndex = WeaponIndex;
		SaveGameInstance->MeshIndex = MeshIndex;
		SaveGameInstance->FlagTextureIndex = GetFlagTextureIndex();
		AGameStateBase* GS = Cast<AGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
		if (GS)
		{
			SaveGameInstance->NumOfPlayers = GS->PlayerArray.Num();
			ALobbyGS* LobbyGS = Cast<ALobbyGS>(GS);
			if (LobbyGS)
			{
				SaveGameInstance->bStartWithCards = LobbyGS->GetStartWithCards();
				SaveGameInstance->bStartWithFlags = LobbyGS->GetStartWithFlags();
				SaveGameInstance->MapInfo = LobbyGS->GetMapInfo();
			}
		}

		UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, PlayerId);
	}
	UPlayerSettings::SetColor1(GetPlayerColor());
	UPlayerSettings::SetColor2(GetFlagColor());
	UPlayerSettings::SaveSettings();
	
	
}

void AMenuPS::SetPlayerColorServer_Implementation(FLinearColor Color)
{
	SetPlayerColor(Color);
}
bool AMenuPS::SetPlayerColorServer_Validate(FLinearColor Color)
{
	return true;
}
void AMenuPS::SetFlagColorServer_Implementation(FLinearColor Color)
{
	SetFlagColor(Color);
}
bool AMenuPS::SetFlagColorServer_Validate(FLinearColor Color)
{
	return true;
}
void AMenuPS::SetWeaponIndexServer_Implementation(int32 Value)
{
	SetWeaponIndex(Value);
}
bool AMenuPS::SetWeaponIndexServer_Validate(int32 Value)
{
	return true;
}
void AMenuPS::SetPlayerName(const FString & S)
{
	
	Super::SetPlayerName(S);
	
}

void AMenuPS::SetPlayerNameServer_Implementation(const FString & S)
{
	SetPlayerName(S);
}
bool AMenuPS::SetPlayerNameServer_Validate(const FString & S)
{
	return true;
}