// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenuPC.h"
#include "Kismet/GameplayStatics.h"
#include "MenuGM.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "InvadedGI.h"
#include "MenuPS.h"
#include "InvadedGS.h"



AMainMenuPC::AMainMenuPC()
{
	bShowMouseCursor = true;
}

void AMainMenuPC::CallServerTravel(const FString & LevelName)
{
	AMenuPS* MenuPS = Cast<AMenuPS>(PlayerState);
	if (MenuPS)
	{
		MenuPS->SaveVariables();
	}
	RemoveWidgets();
	UInvadedGI* InvadedGI = Cast<UInvadedGI>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (InvadedGI)
	{
		UGameplayStatics::OpenLevel(InvadedGI, *LevelName);
	}	
}

void AMainMenuPC::RemoveWidgets()
{
	Super::RemoveWidgets();
	if (UIRef)
		UIRef->RemoveFromParent();
}
