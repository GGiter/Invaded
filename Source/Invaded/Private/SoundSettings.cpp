// Fill out your copyright notice in the Description page of Project Settings.

#include "SoundSettings.h"
#include "..\Public\SoundSettings.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGames/AudioSG.h"

float USoundSettings::MasterVolume = 1.0f;
float USoundSettings::FXVolume = 1.0f;
float USoundSettings::UIVolume = 1.0f;
float USoundSettings::VoiceVolume = 1.0f;
float USoundSettings::MusicVolume = 1.0f;
bool USoundSettings::bMute = false;

void USoundSettings::SetMasterVolume(const float& Value)
{
	MasterVolume = Value;
}
void USoundSettings::SetFXVolume(const float& Value)
{
	FXVolume = Value * MasterVolume;
}

void USoundSettings::SetUIVolume(const float & Value)
{
	UIVolume = Value * MasterVolume;
}

void USoundSettings::SetVoiceVolume(const float & Value)
{
	VoiceVolume = Value * MasterVolume;
}

void USoundSettings::SetMusicVolume(const float & Value)
{
	MusicVolume = MusicVolume * Value;
}

void USoundSettings::Mute()
{
	bMute = true;
}

void USoundSettings::Unmute()
{
	bMute = false;
}

void USoundSettings::SaveSettings()
{
	UAudioSG* SaveGameInstance = Cast<UAudioSG>(UGameplayStatics::CreateSaveGameObject(UAudioSG::StaticClass()));
	if (SaveGameInstance)
	{
		SaveGameInstance->MasterVolume = MasterVolume;
		SaveGameInstance->FXVolume = FXVolume;
		SaveGameInstance->UIVolume = UIVolume;
		SaveGameInstance->VoiceVolume = VoiceVolume;
		SaveGameInstance->MusicVolume = MusicVolume;
		SaveGameInstance->bMute = bMute;
		UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, 0);
	}
	
}

void USoundSettings::LoadSettings()
{
	UAudioSG* LoadGameInstance = Cast<UAudioSG>(UGameplayStatics::CreateSaveGameObject(UAudioSG::StaticClass()));
	if (LoadGameInstance)
	{
		LoadGameInstance = Cast<UAudioSG>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->SaveSlotName, 0));

		if (LoadGameInstance)
		{
			MasterVolume = LoadGameInstance->MasterVolume;
			FXVolume = LoadGameInstance->FXVolume;
			UIVolume = LoadGameInstance->UIVolume;
			VoiceVolume = LoadGameInstance->VoiceVolume;
			MusicVolume = LoadGameInstance->MusicVolume;
			bMute = LoadGameInstance->bMute;
		}
	}
	
}
