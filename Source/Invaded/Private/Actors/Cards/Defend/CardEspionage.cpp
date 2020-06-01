// Fill out your copyright notice in the Description page of Project Settings.

#include "CardEspionage.h"
#include "InvadedData.h"

ACardEspionage::ACardEspionage()
{
	CardName = "Espionage";
}
void ACardEspionage::OnInteract_Implementation(AInvadedCharacter* Caller)
{
	Super::OnInteract_Implementation(Caller);
	UInvadedData* InvadedPS = Caller->GetInvadedPS();
	if (InvadedPS)
	{
		InvadedPS->SetPlayerMode(EPlayerMode::PM_PlayerSelection);
		InvadedPS->SetEnemyAction(EEnemyAction::EA_Move);
	}
}
void ACardEspionage::Activate(AInvadedCharacter* Causer, AInvadedCharacter* Target)
{
	UInvadedData* PlayerPS = Causer->GetLastFocusedPlayer()->GetInvadedPS();
	if (PlayerPS)
	{
		PlayerPS->SetRemainingMoves(2);
		Causer->GetLastFocusedPlayer()->SetCurrentPath(Causer->GetCurrentPath());
		Causer->GetLastFocusedPlayer()->RetraceThePath();
		Causer->ClearLastPath();
		Causer->ClearSelections();
	}
	Causer->GetInvadedPS()->SetPlayerMode(EPlayerMode::PM_Base);
	Causer->GetInvadedPS()->SetEnemyAction(EEnemyAction::EA_Base);
}