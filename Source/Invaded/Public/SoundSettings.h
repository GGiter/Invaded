// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SoundSettings.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API USoundSettings : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Sound")
		static void SetMasterVolume(const float& Value);
	UFUNCTION(BlueprintCallable,Category="Sound")
		static void SetFXVolume(const float& Value);
	UFUNCTION(BlueprintCallable, Category = "Sound")
		static void SetUIVolume(const float& Value);
	UFUNCTION(BlueprintCallable, Category = "Sound")
		static void SetVoiceVolume(const float& Value);
	UFUNCTION(BlueprintCallable, Category = "Sound")
		static void SetMusicVolume(const float& Value);
	UFUNCTION(BlueprintCallable, Category = "Sound")
		static void Mute();
	UFUNCTION(BlueprintCallable, Category = "Sound")
		static void Unmute();

	UFUNCTION(BlueprintCallable, Category = "Sound")
		static void SaveSettings();

	UFUNCTION(BlueprintCallable, Category = "Sound")
		static void LoadSettings();


	UFUNCTION(BlueprintPure, Category = "Sound")
		static float GetFXVolume() { return bMute ? 0.0f : FXVolume; }
	UFUNCTION(BlueprintPure, Category = "Sound")
		static float GetUIVolume() { return bMute ? 0.0f : UIVolume; }
	UFUNCTION(BlueprintPure, Category = "Sound")
		static float GetVoiceVolume() { return bMute? 0.0f : VoiceVolume; }
	UFUNCTION(BlueprintPure, Category = "Sound")
		static float GetMasterVolume() { return bMute? 0.0f : MasterVolume; }
	UFUNCTION(BlueprintPure, Category = "Sound")
		static float GetMusicVolume() { return bMute? 0.0f : MusicVolume; }
	UFUNCTION(BlueprintPure, Category = "Sound")
		static bool IsMute() { return bMute; }
private:

	static float MasterVolume;

	static float FXVolume;

	static float UIVolume;

	static float VoiceVolume;

	static float MusicVolume;

	static bool bMute;
};
