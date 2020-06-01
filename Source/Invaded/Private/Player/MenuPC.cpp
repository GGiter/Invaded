
// Fill out your copyright notice in the Description page of Project Settings.

#include "MenuPC.h"
#include "Kismet/GameplayStatics.h"
#include "MenuGM.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "InvadedGI.h"
#include "MenuPS.h"
#include "InvadedGS.h"
#include "MainMenuPC.h"


AMenuPC::AMenuPC()
{
	bShowMouseCursor = true;
	bReplicates = true;
}
void AMenuPC::SetPlayerColor(FLinearColor Color, int32 PlayerID)
{
	if (Role < ROLE_Authority)
	{
		SetPlayerColorServer(Color, PlayerID);
	}
	else
	{
		AMenuGM* MenuGM = Cast<AMenuGM>(UGameplayStatics::GetGameMode(GetWorld()));
		if (MenuGM)
		{
			MenuGM->SetPlayerColor(PlayerID, Color);
		}
	}
}
void AMenuPC::SetFlagColor(FLinearColor Color, int32 PlayerID)
{
	if (Role < ROLE_Authority)
	{
		SetFlagColorServer(Color, PlayerID);
	}
	else
	{
		AMenuGM* MenuGM = Cast<AMenuGM>(UGameplayStatics::GetGameMode(GetWorld()));
		if (MenuGM)
		{
			MenuGM->SetFlagColor(PlayerID, Color);
		}
	}
}
void AMenuPC::SetFlagColorServer_Implementation(FLinearColor Color, int32 PlayerID)
{
	SetFlagColor(Color, PlayerID);
}
bool AMenuPC::SetFlagColorServer_Validate(FLinearColor Color, int32 PlayerID)
{
	return true;
}
void AMenuPC::SetPlayerColorServer_Implementation(FLinearColor Color, int32 PlayerID)
{
	SetPlayerColor(Color, PlayerID);
}
bool AMenuPC::SetPlayerColorServer_Validate(FLinearColor Color, int32 PlayerID)
{
	return true;
}
void AMenuPC::CallServerTravel(const FString& LevelName)
{
	if (Role < ROLE_Authority)
	{
		CallServerTravelServer(LevelName);
	}
	else
	{
		AGameState* GS = Cast<AGameState>(UGameplayStatics::GetGameState(GetWorld()));
		for (APlayerState* PS : GS->PlayerArray)
		{
			AMenuPS* MenuPS = Cast<AMenuPS>(PS);
			if (MenuPS)
			{
				MenuPS->SaveVariables();
			}

		}
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMenuPC::StaticClass(), Actors);
		for (AActor* Actor : Actors)
		{
			AMenuPC* MenuPC = Cast<AMenuPC>(Actor);
			if (MenuPC)
			{
				MenuPC->RemoveWidgets();
			}
		}
		UInvadedGI* InvadedGI = Cast<UInvadedGI>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (this->IsA<AMainMenuPC>()&&InvadedGI)
		{
			UGameplayStatics::OpenLevel(InvadedGI, *LevelName);
		}
		else if (InvadedGI)
		{
			InvadedGI->ServerTravel(LevelName);
		}
	}
}
void AMenuPC::CallServerTravelServer_Implementation(const FString& LevelName)
{
	CallServerTravel(LevelName);
}
bool AMenuPC::CallServerTravelServer_Validate(const FString& LevelName)
{
	return true;
}
void AMenuPC::RemoveWidgets()
{
	
}