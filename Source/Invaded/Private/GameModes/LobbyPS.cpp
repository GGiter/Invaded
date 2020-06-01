// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyPS.h"
#include "LobbySG.h"
#include "Kismet/GameplayStatics.h"

void ALobbyPS::LoadVariables()
{
	
	UE_LOG(LogTemp, Warning, TEXT("LobbyPS load variables %d"), PlayerId);
	ULobbySG*LoadGameInstance = Cast<ULobbySG>(UGameplayStatics::CreateSaveGameObject(ULobbySG::StaticClass()));
	LoadGameInstance = Cast<ULobbySG>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->SaveSlotName, PlayerId));
	if (LoadGameInstance)
	{		
		if(LoadGameInstance->PlayerName !="")
		SetPlayerName(LoadGameInstance->PlayerName);

		SetPlayerColor(LoadGameInstance->PlayerColor);
		SetFlagColor(LoadGameInstance->FlagColor);
		SetWeaponIndex(LoadGameInstance->WeaponIndex);
		SetMeshIndex(LoadGameInstance->MeshIndex);
	}

}

void ALobbyPS::ClientInitialize(AController * C)
{
	Super::ClientInitialize(C);

	LoadVariables();
}


