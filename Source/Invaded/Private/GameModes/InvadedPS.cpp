// Fill out your copyright notice in the Description page of Project Settings.

#include "InvadedPS.h"
#include "GameFramework/Controller.h"
#include "InvadedGM.h"
#include "UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "LobbySG.h"
#include "Engine/World.h"
#include "Player/InvadedPlayer.h"
#include "InvadedData.h"

void AInvadedPS::ClientInitialize(AController* C)
{
	Super::ClientInitialize(C);
	
	AInvadedPlayer* Player = Cast<AInvadedPlayer>(C->GetPawn());
	if(Player)
	LoadVariables(Player);
		
}

void AInvadedPS::BeginPlay()
{
	Super::BeginPlay();

	
	AInvadedPlayer* Player = Cast<AInvadedPlayer>(GetPawn());
	if (Player)
	{
			LoadVariables(Player);
	}
		

}

void AInvadedPS::LoadVariables(class AInvadedPlayer* Player)
{
	if (Role < ROLE_Authority)
	{
		LoadVariablesServer(Player);
	}
	else
	{

		
		if (Player)
		{
			if (Player->GetInvadedPS())
			{
				ULobbySG*LoadGameInstance = Cast<ULobbySG>(UGameplayStatics::CreateSaveGameObject(ULobbySG::StaticClass()));
				LoadGameInstance = Cast<ULobbySG>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->SaveSlotName, 0));
				if (LoadGameInstance)
				{
					UE_LOG(LogTemp, Warning, TEXT("PlayerID %d"), PlayerId);
					
					if (LoadGameInstance->PlayerColors.Find(PlayerId))
					{
						Player->GetInvadedPS()->SetPlayerColor(LoadGameInstance->PlayerColors[PlayerId]);
					}
						
					if (LoadGameInstance->FlagColors.Find(PlayerId))
					{
						Player->GetInvadedPS()->SetFlagColor(LoadGameInstance->FlagColors[PlayerId]);
						UE_LOG(LogTemp, Warning, TEXT("Logged %d %s"), PlayerId, *LoadGameInstance->FlagColors[PlayerId].ToString());
					}
						
					if (LoadGameInstance->PlayerNames.Find(PlayerId))
					{
						Player->GetInvadedPS()->SetPlayerName(LoadGameInstance->PlayerNames[PlayerId]);
					}
					else
					{
						Player->GetInvadedPS()->SetPlayerName(GetPlayerName());
					}
				
					if (LoadGameInstance->WeaponIndexes.Find(PlayerId))
					{
						Player->GetInvadedPS()->SetWeaponIndex(LoadGameInstance->WeaponIndexes[PlayerId]);
					}

					if (LoadGameInstance->MeshIndexes.Find(PlayerId))
					{
						Player->GetInvadedPS()->SetMeshIndex(LoadGameInstance->MeshIndexes[PlayerId]);
					}

					if (LoadGameInstance->FlagTextureIndexes.Find(PlayerId))
					{
						Player->GetInvadedPS()->SetFlagTextureIndex(LoadGameInstance->FlagTextureIndexes[PlayerId]);
						UE_LOG(LogTemp, Warning, TEXT("Logged %d %d"), PlayerId, LoadGameInstance->FlagTextureIndexes[PlayerId]);
					}

				}

				Player->InitializePlayer();
			}
			else
			{
				
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Player State cast Invaded Player fail"));
		}
		
	}
		

}
void AInvadedPS::LoadVariablesServer_Implementation(class AInvadedPlayer* Player)
{
	LoadVariables(Player);
}
bool AInvadedPS::LoadVariablesServer_Validate(class AInvadedPlayer* Player)
{
	return true;
}
