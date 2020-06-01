// Fill out your copyright notice in the Description page of Project Settings.

#include "CardSpecOps.h"
#include "InvadedData.h"
#include "GridGenerator.h"
#include "CellBase.h"


ACardSpecOps::ACardSpecOps()
{
	CardName = "SpecOps";
}
void ACardSpecOps::OnInteract_Implementation(AInvadedCharacter* Caller)
{
	Super::OnInteract_Implementation(Caller);
	UInvadedData* InvadedPS = Caller->GetInvadedPS();
	if (InvadedPS)
	{
		InvadedPS->SetPlayerMode(EPlayerMode::PM_PlayerSelection);
		InvadedPS->SetEnemyAction(EEnemyAction::EA_MoveToCornerCell);
	}
}
void ACardSpecOps::Activate(AInvadedCharacter* Causer,AInvadedCharacter* Target)
{
	ACellBase* LastFocusedCell = Causer->GetLastFocusedCell();

	Causer->TeleportPlayer(Causer->GetLastFocusedPlayer(), Causer->GetLastFocusedCell()->GetActorLocation() + FVector(0.0f, 0.0f, 100.0f)*AGridGenerator::GetMapScale());
	Causer->GetLastFocusedPlayer()->SetOversteppedCell(Causer->GetLastFocusedCell());

	Causer->ClearSelections();

	Causer->GetInvadedPS()->SetPlayerMode(EPlayerMode::PM_Base);
	Causer->GetInvadedPS()->SetEnemyAction(EEnemyAction::EA_Base);
}