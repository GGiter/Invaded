// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyGM.h"
#include "LobbyPlayer.h"
#include "LobbyPS.h"
#include "InvadedGS.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "InvadedGI.h"
#include "Kismet/GameplayStatics.h"
#include "LobbySG.h"
#include "GameFramework/PlayerController.h"
#include "LobbyGS.h"


void ALobbyGM::LockPlayers()
{
	for (ALobbyPlayer* LobbyPlayer : PlayerArray)
	{
		LobbyPlayer->LockButtons();
		LobbyPlayer->SetRemainingTime(SecondsTillTravel);
	}
	GetWorld()->GetTimerManager().SetTimer(ServerTravelHandle, this, &ALobbyGM::ServerTravel, SecondsTillTravel, false);
}

void ALobbyGM::ServerTravel()
{
	FString MapName="Test";
	AGameState* GS = Cast<AGameState>(UGameplayStatics::GetGameState(GetWorld()));
	ULobbySG*SaveGameInstance = Cast<ULobbySG>(UGameplayStatics::CreateSaveGameObject(ULobbySG::StaticClass()));
	for (APlayerState* PS : GS->PlayerArray)
	{
		AMenuPS* MenuPS = Cast<AMenuPS>(PS);
		if (MenuPS)
		{
			
			SaveGameInstance->NumOfPlayers = GS->PlayerArray.Num();
			ALobbyGS* LobbyGS = Cast<ALobbyGS>(GS);
			if (LobbyGS)
			{
				SaveGameInstance->bStartWithCards = LobbyGS->GetStartWithCards();
				SaveGameInstance->bStartWithFlags = LobbyGS->GetStartWithFlags();
				SaveGameInstance->bStartWithFight = LobbyGS->GetStartWithFight();
				SaveGameInstance->bUseAllFightingTypes = LobbyGS->GetUseAllFightingTypes();
				SaveGameInstance->MapInfo = LobbyGS->GetMapInfo();
				SaveGameInstance->BattleType = LobbyGS->GetBattleType();
				MapName = LobbyGS->GetMapInfo().MapName;
			}
			
			
			SaveGameInstance->FlagColors.Add(MenuPS->PlayerId, MenuPS->GetFlagColor());

			SaveGameInstance->PlayerColors.Add(MenuPS->PlayerId, MenuPS->GetPlayerColor());

			UE_LOG(LogTemp, Warning, TEXT("Saving PlayerName %d"), MenuPS->GetFlagTextureIndex());

			SaveGameInstance->PlayerNames.Add(MenuPS->PlayerId, MenuPS->GetPlayerName());

			SaveGameInstance->WeaponIndexes.Add(MenuPS->PlayerId, MenuPS->GetWeaponIndex());

			SaveGameInstance->MeshIndexes.Add(MenuPS->PlayerId, MenuPS->GetMeshIndex());

			SaveGameInstance->FlagTextureIndexes.Add(MenuPS->PlayerId, MenuPS->GetFlagTextureIndex());
		
		}
	}
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, 0);
	for (ALobbyPlayer* LobbyPlayer : PlayerArray)
	{
		LobbyPlayer->ClearWidgets();
	}
	UInvadedGI* InvadedGI = Cast<UInvadedGI>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (InvadedGI)
	{
		InvadedGI->ExitLobbyLevel();

		FString New= MapName + "?listen";
		GetWorld()->ServerTravel(New);

	}
}

void ALobbyGM::BeginPlay()
{
	Super::BeginPlay();
	UpdateInstalledItemIDs();
}

void ALobbyGM::AddPlayerArray(ALobbyPlayer * Player)
{
	if (PlayerArray.Find(Player) == INDEX_NONE)
		PlayerArray.Add(Player);

	for (ALobbyPlayer* LobbyPlayer : PlayerArray)
	{
		//LobbyPlayer->UpdatePlayerList();
	}
}

void ALobbyGM::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (NewPlayer)
	{
		ALobbyPS* LobbyPS = Cast<ALobbyPS>(NewPlayer->PlayerState);
		if (LobbyPS)
		{
			UE_LOG(LogTemp, Warning, TEXT("Logged"));
			LobbyPS->LoadVariables();
		}
	}
}

void ALobbyGM::UpdateInstalledItemIDs()
{
	ALobbyGS* GS = Cast<ALobbyGS>(UGameplayStatics::GetGameState(GetWorld()));
	if (GS)
	{
		GS->SetInstalledItemIDs(UInvadedGI::InstalledItemIDs);
	}
}

