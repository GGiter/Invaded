// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PlayerSettings.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API UPlayerSettings : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Player")
		static void SaveSettings();

	UFUNCTION(BlueprintCallable, Category = "Player")
		static void LoadSettings();

	UFUNCTION(BlueprintCallable, Category = "Player")
		static void SetColor1(FLinearColor Value);

	UFUNCTION(BlueprintCallable, Category = "Player")
		static void SetColor2(FLinearColor Value);

	UFUNCTION(BlueprintCallable, Category = "Player")
		static void SetZoomSpeed(float Value);

	UFUNCTION(BlueprintPure, Category = "Player")
		static FLinearColor GetColor1() { return Color1; }
	UFUNCTION(BlueprintPure, Category = "Player")
		static FLinearColor GetColor2() { return Color2; }
	UFUNCTION(BlueprintPure, Category = "Player")
		static float GetZoomSpeed() { return ZoomSpeed; }
	UFUNCTION(BlueprintPure, Category = "Player")
		static float GetMaxZoomSpeed() { return MaxZoomSpeed; }

private:
	static FLinearColor Color1;
	static FLinearColor Color2;
	static float ZoomSpeed;
	static float MaxZoomSpeed;
};
