// Fill out your copyright notice in the Description page of Project Settings.


#include "VideoSettings.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGames/VideoSG.h"
#include "Engine/World.h"
#include "GameFramework/GameUserSettings.h"
#include "Engine.h"
#include "Engine/UserInterfaceSettings.h"
#include <IConsoleManager.h>
#include <UObjectGlobals.h>

TArray<int32> UVideoSettings::Graphics = TArray<int32>();
FString UVideoSettings::Resolution = "";
float UVideoSettings::ResolutionX = 1920.0f;
float UVideoSettings::ResolutionY = 1080.0f;

void UVideoSettings::SaveSettings()
{
	UVideoSG* SaveGameInstance = Cast<UVideoSG>(UGameplayStatics::CreateSaveGameObject(UVideoSG::StaticClass()));
	if (SaveGameInstance)
	{
		SaveGameInstance->Graphics = Graphics;
		SaveGameInstance->Resolution = Resolution;

		UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, 0);
	}
}
void UVideoSettings::LoadSettings()
{
	UVideoSG* LoadGameInstance = Cast<UVideoSG>(UGameplayStatics::CreateSaveGameObject(UVideoSG::StaticClass()));
	if (LoadGameInstance)
	{
		LoadGameInstance = Cast<UVideoSG>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->SaveSlotName, 0));

		if (LoadGameInstance)
		{
			Graphics = LoadGameInstance->Graphics;
			Resolution = LoadGameInstance->Resolution;
		}
	}
}

void UVideoSettings::Apply(UEngine* World)
{
	if (Resolution != ""&&Graphics.Num() != 0)
	{
		if (World)
		{
			UGameUserSettings* GUS = World->GameUserSettings;
			if (GUS)
			{
				FString X = "x";
				int32 Index=Resolution.Find(X);
				FString SizeX = "", SizeY = "";
				for (int32 i = 0; i < Index; ++i)
				{
					SizeX += Resolution[i];
				}
				for (int32 i = Index+1; i < Resolution.Len(); ++i)
				{
					SizeY += Resolution[i];
				}
				UE_LOG(LogTemp, Warning, TEXT("Resolution %s %s"),*SizeX,*SizeY);
				GUS->SetScreenResolution(FIntPoint(FCString::Atoi(*SizeX), FCString::Atoi(*SizeY)));
				ResolutionX = FCString::Atoi(*SizeX);
				ResolutionY = FCString::Atoi(*SizeY);

				//ChangeUIScale((ResolutionX / ResolutionY) / (1920.f / 1080.f));

				GUS->SetVSyncEnabled(Graphics[1]);
				GUS->SetViewDistanceQuality(Graphics[2]);
				GUS->SetAntiAliasingQuality(Graphics[3]);
				GUS->SetPostProcessingQuality(Graphics[4]);
				GUS->SetShadowQuality(Graphics[5]);
				GUS->SetTextureQuality(Graphics[6]);
				GUS->SetVisualEffectQuality(Graphics[7]);
				GUS->SetFoliageQuality(Graphics[8]);
				
				switch (Graphics[0])
				{
					case 0:
					{
						GUS->SetFullscreenMode(EWindowMode::WindowedFullscreen);
						break;
					}
					case 1:
					{
						GUS->SetFullscreenMode(EWindowMode::Windowed);
						break;
					}
					case 2:
					{
						GUS->SetFullscreenMode(EWindowMode::Fullscreen);
						break;
					}
				}

				GUS->ApplySettings(true);

			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("VideoSettings GEngine nullptr"));
		}
	}
	
	
	
}

void UVideoSettings::SetGraphics(TArray<int32> Value)
{
	Graphics = Value;
}

void UVideoSettings::SetResolution(FString Value)
{
	Resolution = Value;

	FString X = "x";
	int32 Index = Resolution.Find(X);
	FString SizeX = "", SizeY = "";
	for (int32 i = 0; i < Index; ++i)
	{
		SizeX += Resolution[i];
	}
	for (int32 i = Index + 1; i < Resolution.Len(); ++i)
	{
		SizeY += Resolution[i];
	}

	ResolutionX = FCString::Atoi(*SizeX);
	ResolutionY = FCString::Atoi(*SizeY);
}
void UVideoSettings::ChangeUIScale(float Value)
{
	GetMutableDefault<UUserInterfaceSettings>()->ApplicationScale = Value;
}