// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyPlayer.h"
#include "LobbyPC.h"
#include "GameModes/LobbyGM.h"
#include "GameModes/LobbyPS.h"
#include "Engine/World.h"
#include "UI/InvadedStateMachine.h"

// Sets default values
ALobbyPlayer::ALobbyPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void ALobbyPlayer::BeginPlay()
{
	Super::BeginPlay();
	//LoadVariables();
	if (IsLocallyControlled())
	{
		ALobbyPC* LobbyPC = Cast<ALobbyPC>(GetController());
		if (LobbyPC)
		{
			LobbyPC->AddLobbyUI(LobbyUIClass);
		}
	}
	InitializePlayer();
	
}

void ALobbyPlayer::LoadVariables()
{
	if (Role < ROLE_Authority)
	{
		LoadVariablesServer();
	}
	else
	{
		ALobbyPS* LobbyPS = Cast<ALobbyPS>(GetPlayerState());
		if (LobbyPS)
		{
			LobbyPS->LoadVariables();
		}
	}
}

void ALobbyPlayer::LoadVariablesServer_Implementation()
{
	LoadVariables();
}
bool ALobbyPlayer::LoadVariablesServer_Validate()
{
	return true;
}
void ALobbyPlayer::InitializePlayer()
{
	if (Role < ROLE_Authority)
	{
		InitializePlayerServer();
	}
	else
	{
		ALobbyGM* LobbyGM = GetWorld()->GetAuthGameMode<ALobbyGM>();
		if (LobbyGM)
		{
			LobbyGM->AddPlayerArray(this);
		}
	}
}

void ALobbyPlayer::UpdatePlayerListClient_Implementation()
{
	UpdatePlayerList();
}
bool ALobbyPlayer::UpdatePlayerListClient_Validate()
{
	return true;
}
void ALobbyPlayer::InitializePlayerServer_Implementation()
{
	InitializePlayer();
}
bool ALobbyPlayer::InitializePlayerServer_Validate()
{
	return true;
}
void ALobbyPlayer::LockButtons()
{
	if (Role < ROLE_Authority || IsLocallyControlled())
	{
		ALobbyPC* LobbyPC = Cast<ALobbyPC>(GetController());
		if (LobbyPC)
		{
			LobbyPC->LockButtons();
		}
	}
	else
	{
		LockButtonsClient();
	}
}

void ALobbyPlayer::UpdatePlayerList()
{
	if (Role < ROLE_Authority || IsLocallyControlled())
	{
		ALobbyPC* LobbyPC = Cast<ALobbyPC>(GetController());
		if (LobbyPC)
		{
			LobbyPC->UpdatePlayerList();
		}
	}
	else
	{
		UpdatePlayerListClient();
	}
}

void ALobbyPlayer::LockButtonsClient_Implementation()
{
	LockButtons();
}
bool ALobbyPlayer::LockButtonsClient_Validate()
{
	return true;
}
void ALobbyPlayer::ClearWidgets()
{
	if (Role < ROLE_Authority || IsLocallyControlled())
	{
		ALobbyPC* LobbyPC = Cast<ALobbyPC>(GetController());
		if (LobbyPC)
		{
			if (LobbyPC->StateMachineWidget)
				LobbyPC->StateMachineWidget->RemoveFromParent();

			BP_OnClearWidgets();
		}
		
	}
	else
	{
		ClearWidgetsClient();
	}
}

void ALobbyPlayer::SetRemainingTime(float Value)
{
	if (Role < ROLE_Authority || IsLocallyControlled())
	{
		ALobbyPC* LobbyPC = Cast<ALobbyPC>(GetController());
		if (LobbyPC)
		{
			LobbyPC->SetRemainingTime(Value);
		}

	}
	else
	{
		SetRemainingTimeClient(Value);
	}
}

void ALobbyPlayer::ClearWidgetsClient_Implementation()
{
	ClearWidgets();
}
bool ALobbyPlayer::ClearWidgetsClient_Validate()
{
	return true;
}
void ALobbyPlayer::SetRemainingTimeClient_Implementation(float Value)
{
	SetRemainingTime(Value);
}
bool ALobbyPlayer::SetRemainingTimeClient_Validate(float Value)
{
	return true;
}
// Called every frame
void ALobbyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ALobbyPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

