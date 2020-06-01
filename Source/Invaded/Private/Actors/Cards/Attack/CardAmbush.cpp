// Fill out your copyright notice in the Description page of Project Settings.

#include "CardAmbush.h"
#include "InvadedData.h"
#include "InvadedGM.h"
#include "Engine/World.h"

ACardAmbush::ACardAmbush()
{
	CardName = "Ambush";
}
void ACardAmbush::OnInteract_Implementation(AInvadedCharacter* Caller)
{
	Super::OnInteract_Implementation(Caller);
	UInvadedData* InvadedPS = Caller->GetInvadedPS();
	if (InvadedPS)
	{
		InvadedPS->SetPlayerMode(EPlayerMode::PM_PlayerSelection);
		InvadedPS->SetEnemyAction(EEnemyAction::EA_LoseTurn);
	}
}
void ACardAmbush::Activate(AInvadedCharacter* Causer, AInvadedCharacter* Target)
{
	AInvadedGM* InvadedGM = GetWorld()->GetAuthGameMode<AInvadedGM>();
	if (InvadedGM)
	{
		InvadedGM->SkipPlayerTurn(Causer->GetLastFocusedPlayer());
		Causer->GetInvadedPS()->SetPlayerMode(EPlayerMode::PM_Base);
		Causer->GetInvadedPS()->SetEnemyAction(EEnemyAction::EA_Base);
		Causer->ClearSelections();
	}
}

