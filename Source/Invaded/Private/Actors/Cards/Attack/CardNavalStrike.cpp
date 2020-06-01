// Fill out your copyright notice in the Description page of Project Settings.

#include "CardNavalStrike.h"
#include "InvadedGM.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

// Sets default values
ACardNavalStrike::ACardNavalStrike()
{
	CardName = "Naval Strike";
}

void ACardNavalStrike::OnInteract_Implementation(AInvadedCharacter * Caller)
{
	Super::OnInteract_Implementation(Caller);

	SpawnEffect(Caller, nullptr);
}
void ACardNavalStrike::Activate(AInvadedCharacter* Causer, AInvadedCharacter* Target)
{
	AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (InvadedGM)
	{
		InvadedGM->ResetFlags();
	}
}