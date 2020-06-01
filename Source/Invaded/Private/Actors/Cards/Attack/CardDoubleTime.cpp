// Fill out your copyright notice in the Description page of Project Settings.

#include "CardDoubleTime.h"
#include "GameModes/InvadedGM.h"
#include "GameModes/InvadedGS.h"
#include "GameModes/InvadedData.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

ACardDoubleTime::ACardDoubleTime()
{
	CardName = "Double Time";
}
void ACardDoubleTime::OnInteract_Implementation(AInvadedCharacter* Caller)
{
	Super::OnInteract_Implementation(Caller);
	
	SpawnEffect(Caller, nullptr);
}
void ACardDoubleTime::Activate(AInvadedCharacter* Causer, AInvadedCharacter* Target)
{
	AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
	AInvadedGS* InvadedGS = Cast<AInvadedGS>(UGameplayStatics::GetGameState(GetWorld()));
	if (InvadedGM&&InvadedGS)
	{
		InvadedGS->GetCurrentPlayer()->SetAdditionalThrow(true);
		InvadedGM->AdditionalThrow();
	}
}