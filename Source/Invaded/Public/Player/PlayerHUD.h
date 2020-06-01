// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Button.h"
#include "UI/InvadedMenuState.h"
#include "PlayerHUD.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EMessageType : uint8
{
	MT_Normal UMETA(DisplayName = "Normal"),
	MT_Warning UMETA(DisplayName = "Warning")
};
USTRUCT(BlueprintType)
struct FSChatMessage // Struct to hold the message data to be passed between classes
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY() // UProperty means this variable will be replicated
		EMessageType Type;

	UPROPERTY(BlueprintReadWrite)
		FLinearColor UsernameColor;

	UPROPERTY(BlueprintReadWrite)
		FText Username;

	UPROPERTY(BlueprintReadWrite)
		FText Text;

	FText Timestamp; // Dont replicate time because we can set it locally once we receive the struct

	double Created;

	void Init(EMessageType NewType, FText NewUsername, FText NewText, FLinearColor NewUsernameColor) // Assign only the vars we wish to replicate
	{
		Type = NewType;
		Username = NewUsername;
		Text = NewText;
		UsernameColor = NewUsernameColor;
	}
	void SetTime(FText NewTimestamp, double NewCreated)
	{
		Timestamp = NewTimestamp;
		Created = NewCreated;
	}
	void Destroy()
	{
		Type = EMessageType::MT_Normal;
		Username.GetEmpty();
		Text.GetEmpty();
		Timestamp.GetEmpty();
		UsernameColor = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);
		Created = NULL;
	}
};

UCLASS()
class INVADED_API APlayerHUD : public AHUD
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "User")
		void AddMessage(EMessageType Type, const FString& Username, const FString& Text, const bool Replicate);
	UFUNCTION(BlueprintImplementableEvent, Category = "User")
		void AddMessageBP(FSChatMessage Message);
	UFUNCTION(BlueprintImplementableEvent, Category = "Bugs")
		void ReportBugComplete(bool bWasSuccessful);
	UFUNCTION(BlueprintCallable, Category = "Bugs")
		void ReportBug(FString Name, FString Description);
	UFUNCTION(BlueprintCallable, Category = "Widgets")
		void Update();
	UFUNCTION(BlueprintCallable, Category = "Widgets")
		void OpenNotificationBox(FText Text, float Time);
	UFUNCTION(BlueprintCallable, Category = "Widgets")
		void OpenOneButtonDialogBox(FText Text, EMenuStackFlags MenuStackFlags);
	UFUNCTION(BlueprintCallable, Category = "Widgets")
		TArray<UButton*> OpenTwoButtonDialogBox(FText Text, EMenuStackFlags MenuStackFlags);
	
	
};
