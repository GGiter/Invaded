// Fill out your copyright notice in the Description page of Project Settings.

#include "CardFortify.h"
#include "InvadedData.h"
#include "Player/InvadedPlayer.h"

ACardFortify::ACardFortify()
{
	AdditionalMoves = 2;
	CardName = "Fortify";
}
void ACardFortify::OnInteract_Implementation(AInvadedCharacter*Caller)
{
	Super::OnInteract_Implementation(Caller);
	SpawnEffect(Caller, nullptr);
}
void ACardFortify::Activate(AInvadedCharacter* Causer, AInvadedCharacter* Target)
{
	UInvadedData* InvadedPS = Causer->GetInvadedPS();
	if (InvadedPS)
	{
		InvadedPS->AddRemainingMoves(AdditionalMoves);
	}
}
