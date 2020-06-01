// Fill out your copyright notice in the Description page of Project Settings.

#include "InvadedGS.h"
#include "InvadedData.h"
#include "UnrealNetwork.h"
#include "LobbySG.h"
#include "Kismet/GameplayStatics.h"
#include "InvadedGM.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"


void AInvadedGS::BeginPlay()
{
	Super::BeginPlay();

	TimeRemaining = -1;
	GetWorld()->GetTimerManager().SetTimer(TimeUpdateHandle, this, &AInvadedGS::UpdateTime, 1.0f, true);
}
void AInvadedGS::UpdateTime()
{
	if (Role == ROLE_Authority&&CurrentPlayerState)
	{
		if (CurrentPlayerState->GetPlayerMode() == EPlayerMode::PM_Base||CurrentPlayerState->GetPlayerMode() == EPlayerMode::PM_UI)
		{
			TimeRemaining--;
			if (TimeRemaining == 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("Time run out"));

				AInvadedGM* InvadedGM = GetWorld()->GetAuthGameMode<AInvadedGM>();
				if (InvadedGM)
				{
					InvadedGM->OnEndTurn();
				}

			}
		}
		
	}
}
void AInvadedGS::SetMaxTurnTime(float Value)
{
	MaxTurnTime = Value;
}
void AInvadedGS::SetCurrentPlayer(UInvadedData* PlayerState)
{
	TimeRemaining = MaxTurnTime;
	if (CurrentPlayerState)
	{
		CurrentPlayerState->SetIsCurrentPlayer(false);
	}
	CurrentPlayerState = PlayerState;
	if (CurrentPlayerState)
	{
		CurrentPlayerState->SetIsCurrentPlayer(true);
		CurrentPlayerIndex++;
	}
	
}
void AInvadedGS::SetCurrentTurn(const uint8& Value)
{
	CurrentTurn = Value;
}
void AInvadedGS::SetCurrentPlayerIndex(const uint8& Value)
{
	CurrentPlayerIndex = Value;
	if (Value==0&&CurrentPlayerState)
	{
		CurrentPlayerState->SetIsCurrentPlayer(false);
	}
}
void AInvadedGS::SetGridGenerator(class AGridGenerator* Value)
{
	GridGenerator = Value;
}
void AInvadedGS::LoadVariables(int32 PlayerID=0)
{
	ULobbySG* LoadGameInstance = Cast<ULobbySG>(UGameplayStatics::CreateSaveGameObject(ULobbySG::StaticClass()));
	LoadGameInstance = Cast<ULobbySG>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->SaveSlotName, PlayerID));
	if (LoadGameInstance)
	{
		NumOfPlayers = LoadGameInstance->NumOfPlayers;

		AInvadedGM* InvadedGM = GetWorld()->GetAuthGameMode<AInvadedGM>();
		if (InvadedGM)
		{
			LoadGameInstance->MapInfo.MapScale = InvadedGM->GetMapScale();
			InvadedGM->SetStartWithCards(LoadGameInstance->bStartWithCards);
			InvadedGM->SetStartWithFlags(LoadGameInstance->bStartWithFlags);
			InvadedGM->SetStartWithFight(LoadGameInstance->bStartWithFight);
			InvadedGM->SetUseAllFightingTypes(LoadGameInstance->bUseAllFightingTypes);
			InvadedGM->SetMapScale(LoadGameInstance->MapInfo.MapScale);
			BattleType = LoadGameInstance->BattleType;
			MapInfo = LoadGameInstance->MapInfo;
		}
	}
	
	
}
void AInvadedGS::SetNumberOfPlayers(int32 Value)
{
	NumOfPlayers = Value;
}
void AInvadedGS::SetCurrentNumOfPlayers(int32 Value)
{
	CurrentNumOfPlayers = Value;
}
void AInvadedGS::SetNumberOfAlivePlayers(int32 Value)
{
	NumberOfAlivePlayers = Value;
}
void AInvadedGS::AddInvadedPlayerState(class UInvadedData* Value)
{
	InvadedPlayerStates.Add(Value);
}
void AInvadedGS::RemoveInvadedPlayerState(UInvadedData * Value)
{
	InvadedPlayerStates.Remove(Value);
}
void AInvadedGS::SetPlayerStates(TArray<class UInvadedData*> Value)
{
	InvadedPlayerStates = Value;
}
void AInvadedGS::SetMapScale(float Value)
{
	MapScale = Value;
	MapInfo.MapScale = Value;
}
void AInvadedGS::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInvadedGS, CurrentPlayerState);
	DOREPLIFETIME(AInvadedGS, CurrentPlayerIndex);
	DOREPLIFETIME(AInvadedGS, CurrentTurn);
	DOREPLIFETIME(AInvadedGS, GridGenerator);
	DOREPLIFETIME(AInvadedGS, NumOfPlayers);
	DOREPLIFETIME(AInvadedGS, CurrentNumOfPlayers);
	DOREPLIFETIME(AInvadedGS, CurrentPlayerIndex);
	DOREPLIFETIME(AInvadedGS, NumberOfAlivePlayers);
	DOREPLIFETIME(AInvadedGS, InvadedPlayerStates);
	DOREPLIFETIME(AInvadedGS, MapScale);
	DOREPLIFETIME(AInvadedGS, TimeRemaining);
	DOREPLIFETIME(AInvadedGS, MaxTurnTime);
	DOREPLIFETIME(AInvadedGS, MapInfo);
	DOREPLIFETIME(AInvadedGS, BattleType);
}


