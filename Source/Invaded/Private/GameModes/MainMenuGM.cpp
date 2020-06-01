// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenuGM.h"
#include "MainMenuPC.h"
#include "UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "InvadedGS.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "InvadedGI.h"
#include "Kismet/GameplayStatics.h"
#include "LobbySG.h"
#include "GameFramework/PlayerController.h"
#include "MenuPS.h"

void AMainMenuGM::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetWorld()->GetNetMode() != NM_DedicatedServer)
	{
	
	}
	else
	{
		BP_DedicatedServer();
	}
	
	
	
}
void AMainMenuGM::ServerTravel()
{


	AGameState* GS = Cast<AGameState>(UGameplayStatics::GetGameState(GetWorld()));
	ULobbySG*SaveGameInstance = Cast<ULobbySG>(UGameplayStatics::CreateSaveGameObject(ULobbySG::StaticClass()));
	for (APlayerState* PS : GS->PlayerArray)
	{
		AMenuPS* MenuPS = Cast<AMenuPS>(PS);
		if (MenuPS)
		{
			SaveGameInstance->NumOfPlayers = GS->PlayerArray.Num();

			SaveGameInstance->bStartWithCards = false;
			SaveGameInstance->bStartWithFlags = false;
			SaveGameInstance->bStartWithFight = false;
			FMapInfo MapInfo;
			MapInfo.MapName = "Test";
			MapInfo.MapScale = 0.5f;
			SaveGameInstance->MapInfo = MapInfo;
	

			SaveGameInstance->FlagColors.Add(MenuPS->PlayerId, MenuPS->GetFlagColor());

			SaveGameInstance->PlayerColors.Add(MenuPS->PlayerId, MenuPS->GetPlayerColor());


			SaveGameInstance->PlayerNames.Add(MenuPS->PlayerId, MenuPS->GetPlayerName());

			SaveGameInstance->WeaponIndexes.Add(MenuPS->PlayerId, MenuPS->GetWeaponIndex());

			SaveGameInstance->MeshIndexes.Add(MenuPS->PlayerId, MenuPS->GetMeshIndex());

		}
	}
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, 0);
	

}

