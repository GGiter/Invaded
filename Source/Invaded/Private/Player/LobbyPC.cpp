// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyPC.h"
#include "Kismet/GameplayStatics.h"
#include "LobbyGM.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "InvadedGI.h"
#include "LobbyPS.h"
#include "InvadedGS.h"
#include "LobbyGS.h"
#include "UnrealNetwork.h"
#include "LobbyPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "UI/LobbyWidget.h"
#include "UI/InvadedStateMachine.h"

ALobbyPC::ALobbyPC()
{
	bShowMouseCursor = true;
	bReplicates = true;
	TestString = FString::FromInt(FMath::FRandRange(0.0f, 100.0f));
	bAutoManageActiveCameraTarget = false;
	CameraTag = "MainCamera";
}
void ALobbyPC::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> Actors;

	UGameplayStatics::GetAllActorsWithTag(GetWorld(), *CameraTag, Actors);
	
	for (AActor* Actor : Actors)
	{
		SetViewTargetWithBlend(Actor);
		break;
	}
}
void ALobbyPC::UpdatePlayerList_Implementation()
{
	if (LobbyWidget)
	{
		//LobbyWidget->UpdatePlayerList();
	}
}
bool ALobbyPC::UpdatePlayerList_Validate()
{
	return true;
}
void ALobbyPC::LockButtons_Implementation()
{
	if (LobbyWidget)
	{
		LobbyWidget->LockButtons();
	}
}
bool ALobbyPC::LockButtons_Validate()
{
	return true;
}
void ALobbyPC::SetRemainingTime_Implementation(float Time)
{
	if (LobbyWidget)
	{
		LobbyWidget->SetRemaingTime(Time);
	}
}
bool ALobbyPC::SetRemainingTime_Validate(float Time)
{
	return true;
}
void ALobbyPC::RemoveWidgets()
{
	Super::RemoveWidgets();
	if (LobbyWidget)
		LobbyWidget->RemoveFromParent();
}
void ALobbyPC::AddLobbyUI_Implementation(TSubclassOf<UUserWidget> LobbyUIClass)
{

	for (TObjectIterator<UUserWidget> Itr; Itr; ++Itr)
	{
		UUserWidget* LiveWidget = *Itr;

		
		if (!LiveWidget->GetWorld())
		{
			continue;
		}
		else
		{
			LiveWidget->RemoveFromParent();
		}
	}

	StateMachineWidget = CreateWidget<UInvadedStateMachine>(GetWorld(), LobbyUIClass);
	if (StateMachineWidget)
	{
		StateMachineWidget->AddToViewport();
		LobbyWidget = Cast<ULobbyWidget>(StateMachineWidget->FindStateByName("Lobby"));
	}
	
}
bool ALobbyPC::AddLobbyUI_Validate(TSubclassOf<UUserWidget> LobbyUIClass)
{
	return true;
}
void ALobbyPC::SetPlayerReadyServer_Implementation( bool  Value)
{
	SetPlayerReady(Value);
}
bool ALobbyPC::SetPlayerReadyServer_Validate( bool  Value)
{
	return true;
}
void ALobbyPC::SetPlayerReady(const bool& Value)
{
	if (Role < ROLE_Authority)
	{
		SetPlayerReadyServer(Value);
	}
	else
	{
		ALobbyGS* LobbyGS=Cast<ALobbyGS>(UGameplayStatics::GetGameState(GetWorld()));
		if (LobbyGS)
		{
			
			LobbyGS->SetIsPlayerReady(PlayerState, Value);
		}
	}
}

void ALobbyPC::UpdateServerPlayerArray()
{
	if (Role < ROLE_Authority)
	{
		UpdateServerPlayerArrayServer();
	}
	else
	{
		ServerPlayerArray = UGameplayStatics::GetGameState(GetWorld())->PlayerArray;
	}
}
void ALobbyPC::UpdateServerPlayerArrayServer_Implementation()
{
	UpdateServerPlayerArray();
}
bool ALobbyPC::UpdateServerPlayerArrayServer_Validate()
{
	return true;
}
void ALobbyPC::SetMapInfo(FMapInfo MapInfo)
{
	if (Role < ROLE_Authority)
	{
		SetMapInfoServer(MapInfo);
	}
	else
	{
		ALobbyGS* LobbyGS = GetWorld()->GetGameState<ALobbyGS>();
		if (LobbyGS)
		{
			LobbyGS->SetMapInfo(MapInfo);
		}
	}
}
void ALobbyPC::ExitSession()
{
	if (Role < ROLE_Authority)
	{
		ExitSessionServer();
	}
	else
	{
		UGameplayStatics::GetGameState(GetWorld())->PlayerArray.Remove(PlayerState);

		TArray<AActor*> OutActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALobbyPlayer::StaticClass(), OutActors);
		for (AActor* Actor : OutActors)
		{
			ALobbyPlayer* LobbyPlayer = Cast<ALobbyPlayer>(Actor);
			if (LobbyPlayer&&LobbyPlayer != GetPawn())
			{
				LobbyPlayer->UpdatePlayerList();
			}
		}
	}
}
void ALobbyPC::ExitSessionServer_Implementation()
{
	ExitSession();
}
bool ALobbyPC::ExitSessionServer_Validate()
{
	return true;
}
void ALobbyPC::SetMapInfoServer_Implementation(FMapInfo MapInfo)
{
	SetMapInfo(MapInfo);
}
bool ALobbyPC::SetMapInfoServer_Validate(FMapInfo MapInfo)
{
	return true;
}
void ALobbyPC::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyPC, ServerPlayerArray);

}