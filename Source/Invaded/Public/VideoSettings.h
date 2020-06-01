// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "VideoSettings.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API UVideoSettings : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Video")
		static void SaveSettings();

	UFUNCTION(BlueprintCallable, Category = "Video")
		static void LoadSettings();

	UFUNCTION(BlueprintCallable, Category = "Video")
		static void SetGraphics(TArray<int32> Value);

	UFUNCTION(BlueprintCallable, Category = "Video")
		static void SetResolution(FString Value);

	UFUNCTION(BlueprintCallable, Category = "Video")
		static void Apply(class UEngine* World);

	UFUNCTION(BlueprintCallable, Category = "Video")
		static void ChangeUIScale(float Value);

	UFUNCTION(BlueprintPure, Category = "Video")
		static TArray<int32> GetGraphics() { return Graphics; }
	UFUNCTION(BlueprintPure, Category = "Video")
		static FString GetResolution() { return Resolution; }
	UFUNCTION(BlueprintPure, Category = "Video")
		static float GetResolutionX() { return ResolutionX; }
	UFUNCTION(BlueprintPure, Category = "Video")
		static float GetResolutionY() { return ResolutionY; }
private:

	static	TArray<int32> Graphics;

	
	static	FString Resolution;

	static float ResolutionX;

	static float ResolutionY;
};
