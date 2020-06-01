// Fill out your copyright notice in the Description page of Project Settings.

#include "CardNuke.h"
#include "Kismet/GameplayStatics.h"
#include "InvadedGM.h"
#include "Engine/World.h"

ACardNuke::ACardNuke()
{
	CardName = "Nuke";
}
void ACardNuke::OnInteract_Implementation(AInvadedCharacter * Caller)
{
	Super::OnInteract_Implementation(Caller);

	SpawnEffect(Caller, nullptr);
}
void ACardNuke::Activate(AInvadedCharacter* Causer,AInvadedCharacter* Target)
{
	AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (InvadedGM)
	{
		InvadedGM->ResetPlayerLocations();
	}
}
