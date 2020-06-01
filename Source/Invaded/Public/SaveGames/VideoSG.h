// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "VideoSG.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API UVideoSG : public USaveGame
{
	GENERATED_BODY()
public:
	UVideoSG();

	FString SaveSlotName;

	uint32 UserIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int32> Graphics;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Resolution;

};
