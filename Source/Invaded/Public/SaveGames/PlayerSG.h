// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PlayerSG.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API UPlayerSG : public USaveGame
{
	GENERATED_BODY()
public:
	UPlayerSG();

	FString SaveSlotName;

	uint32 UserIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor Color1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor Color2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ZoomSpeed;
};
