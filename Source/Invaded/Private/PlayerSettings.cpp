// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSettings.h"
#include "SaveGames/PlayerSG.h"
#include "Kismet/GameplayStatics.h"

FLinearColor UPlayerSettings::Color1 = FLinearColor(0, 0, 0, 1);
FLinearColor UPlayerSettings::Color2 = FLinearColor(1, 1, 1, 1);
float UPlayerSettings::ZoomSpeed = 1.0f;
float UPlayerSettings::MaxZoomSpeed = 3.0f;


void UPlayerSettings::SaveSettings()
{
	UPlayerSG* SaveGameInstance = Cast<UPlayerSG>(UGameplayStatics::CreateSaveGameObject(UPlayerSG::StaticClass()));
	if (SaveGameInstance)
	{
		SaveGameInstance->Color1 = Color1;
		SaveGameInstance->Color2 = Color2;
		SaveGameInstance->ZoomSpeed = ZoomSpeed;

		UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, 0);
	}
}
void UPlayerSettings::LoadSettings()
{
	UPlayerSG* LoadGameInstance = Cast<UPlayerSG>(UGameplayStatics::CreateSaveGameObject(UPlayerSG::StaticClass()));
	if (LoadGameInstance)
	{
		LoadGameInstance = Cast<UPlayerSG>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->SaveSlotName, 0));

		if (LoadGameInstance)
		{
			Color1 = LoadGameInstance->Color1;
			Color2 = LoadGameInstance->Color2;
			ZoomSpeed = LoadGameInstance->ZoomSpeed;
		}
	}
}
void UPlayerSettings::SetColor1(FLinearColor Value)
{
	Color1 = Value;
}
void UPlayerSettings::SetColor2(FLinearColor Value)
{
	Color2 = Value;
}
void UPlayerSettings::SetZoomSpeed(float Value)
{
	ZoomSpeed = Value;
}