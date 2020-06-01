// Fill out your copyright notice in the Description page of Project Settings.

#include "CellAction.h"
#include "Kismet/GameplayStatics.h"
#include "GameModes/InvadedGM.h"
#include "UnrealNetwork.h"
#include "Engine/World.h"
#include "CardBase.h"
#include "InvadedCharacter.h"




void ACellAction::Activate(APawn*Caller)
{
	Super::Activate(Caller);
	DrawCard();
	AInvadedCharacter* Character = Cast<AInvadedCharacter>(Caller);
	if (Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("Adding Card %s"), *CardRef->GetName());
		CardRef->SetOwner(Caller);
		Character->AddCard(CardRef);
	}
}
void ACellAction::OnInteract_Implementation(APawn*Caller)
{
	Super::OnInteract_Implementation(Caller);	
}

void ACellAction::DrawCard_Implementation()
{
	AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (InvadedGM)
	{	
		FActorSpawnParameters SpawnParams;
		if (CellType==ECellType::CT_Attack)
		{
			CardRef = GetWorld()->SpawnActor<ACardBase>(InvadedGM->DrawAttackCard(), FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f), SpawnParams);
		}
		else if (CellType == ECellType::CT_Defense)
		{
			CardRef = GetWorld()->SpawnActor<ACardBase>(InvadedGM->DrawDefensiveCard(), FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f), SpawnParams);
		}
	}
}
bool ACellAction::DrawCard_Validate()
{
	return true;
}
void ACellAction::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACellAction, CardRef);

}



