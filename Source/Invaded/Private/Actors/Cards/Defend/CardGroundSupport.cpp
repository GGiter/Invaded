// Fill out your copyright notice in the Description page of Project Settings.

#include "CardGroundSupport.h"
#include "InvadedData.h"
#include "InvadedGM.h"
#include "Engine/World.h"


ACardGroundSupport::ACardGroundSupport()
{
	CardName = "Ground Support";
}
void ACardGroundSupport::OnInteract_Implementation(AInvadedCharacter* Caller)
{
	Super::OnInteract_Implementation(Caller);
	UInvadedData* InvadedPS = Caller->GetInvadedPS();
	if (InvadedPS)
	{
		InvadedPS->SetPlayerMode(EPlayerMode::PM_PlayerSelection);
		InvadedPS->SetEnemyAction(EEnemyAction::EA_LoseBattle);
	}
}
void ACardGroundSupport::Activate(AInvadedCharacter* Causer, AInvadedCharacter* Target)
{
	AInvadedGM* InvadedGM = GetWorld()->GetAuthGameMode<AInvadedGM>();
	if (InvadedGM)
	{
		Causer->ClearSelections();
		InvadedGM->AddNextBattleLoser(Causer->GetLastFocusedPlayer());
		Causer->GetInvadedPS()->SetPlayerMode(EPlayerMode::PM_Base);
		Causer->GetInvadedPS()->SetEnemyAction(EEnemyAction::EA_Base);
	}
}