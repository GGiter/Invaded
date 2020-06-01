// Fill out your copyright notice in the Description page of Project Settings.

#include "CardCloseAirSupport.h"
#include "Kismet/GameplayStatics.h"
#include "InvadedPlayer.h"
#include "InvadedGM.h"
#include "Engine/World.h"

ACardCloseAirSupport::ACardCloseAirSupport()
{
	CardName = "Close Air Support";
}
void ACardCloseAirSupport::OnInteract_Implementation(AInvadedCharacter* Caller)
{
	Super::OnInteract_Implementation(Caller);

	SpawnEffect(Caller, nullptr);
}
void ACardCloseAirSupport::Activate(AInvadedCharacter* Causer, AInvadedCharacter* Target)
{
	if (Causer)
	{
		AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
		if (InvadedGM)
		{
			InvadedGM->AddNextBattleWinner(Causer);
		}
	}
}
