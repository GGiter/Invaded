// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AudioSG.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API UAudioSG : public USaveGame
{
	GENERATED_BODY()
public:
	UAudioSG();

	
	FString SaveSlotName;
	
	uint32 UserIndex;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float MasterVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FXVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float UIVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float VoiceVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MusicVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMute;
};
