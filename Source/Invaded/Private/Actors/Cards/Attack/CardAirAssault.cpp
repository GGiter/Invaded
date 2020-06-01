// Fill out your copyright notice in the Description page of Project Settings.

#include "CardAirAssault.h"
#include "InvadedData.h"
#include "CellBase.h"

ACardAirAssault::ACardAirAssault()
{
	CardName = "Air Assault";
	bReplicates = true;
}
void ACardAirAssault::OnInteract_Implementation(AInvadedCharacter * Caller)
{
	Super::OnInteract_Implementation(Caller);
	UInvadedData* InvadedPS = Caller->GetInvadedPS();
	if (InvadedPS)
	{
		InvadedPS->SetPlayerMode(EPlayerMode::PM_CornerCellSelection);
	}
}
void ACardAirAssault::Activate(AInvadedCharacter* Causer, AInvadedCharacter* Target)
{
	Causer->TeleportPlayer(Causer, Causer->GetLastFocusedCell()->GetActorLocation() + FVector(0.0f, 0.0f, 100.0f));
	Causer->SetOversteppedCell(Causer->GetLastFocusedCell());
	Causer->ClearSelections();
	Causer->GetInvadedPS()->SetPlayerMode(EPlayerMode::PM_Base);
	Causer->GetInvadedPS()->SetEnemyAction(EEnemyAction::EA_Base);
}
