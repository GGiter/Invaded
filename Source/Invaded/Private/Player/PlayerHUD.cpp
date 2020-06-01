// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerHUD.h"
#include "InvadedData.h"
#include "InvadedCharacter.h"
#include "Player/InvadedPlayer.h"
#include "..\..\Public\Player\PlayerHUD.h"
#include "BugReporter.h"
#include "UI/MainGameWidget.h"
#include "UI/NotificationBox.h"
#include "UI/InvadedStateMachine.h"
#include "UI/OneButtonDialogBox.h"
#include "UI/TwoButtonsDialogBox.h"


void APlayerHUD::AddMessage(EMessageType Type, const FString& Username, const FString& Text, const bool Replicate)
{
	APlayerController* PlayerCon = Cast<APlayerController>(GetOwningPlayerController());
	
	AInvadedCharacter* InvadedCharacter = Cast<AInvadedCharacter>(GetOwningPawn());
	if (!PlayerCon||!InvadedCharacter)
		return;

	FSChatMessage NewMessage;
	NewMessage.Init(Type, FText::FromString(Username), FText::FromString(Text),InvadedCharacter->GetInvadedPS()->GetPlayerColor()); 
	
	if (Replicate)
	{
		InvadedCharacter->GetInvadedPS()->UserChatServer(NewMessage); 
	}
	else
		AddMessageBP(NewMessage);
		 
}

void APlayerHUD::ReportBug(FString Name, FString Description)
{
	UBugReporter* BugReporter = NewObject<UBugReporter>(this, UBugReporter::StaticClass());
	if (BugReporter)
	{
		BugReporter->ReportBug(Name, Description, this);
	}
}

void APlayerHUD::Update()
{
	AInvadedPlayer* Player = Cast<AInvadedPlayer>(GetOwningPawn());
	if (Player)
	{
		if (Player->GetInvadedStateMachine())
		{
			UMainGameWidget* MainGameWidget=Cast<UMainGameWidget>(Player->GetInvadedStateMachine()->FindStateByName("GameUI"));
			if (MainGameWidget)
			{
				MainGameWidget->Update();
			}
		}
	}
}

void APlayerHUD::OpenNotificationBox(FText Text, float Time)
{
	AInvadedPlayer* Player = Cast<AInvadedPlayer>(GetOwningPawn());
	if (Player)
	{
		if (Player->GetInvadedStateMachine())
		{
			UNotificationBox* NotificationBox = Cast<UNotificationBox>(Player->GetInvadedStateMachine()->FindStateByName("NotificationBox"));
			if (NotificationBox)
			{
				NotificationBox->Open(Text, Time);
			}
		}
	}
}

void APlayerHUD::OpenOneButtonDialogBox(FText Text,EMenuStackFlags MenuStackFlags)
{
	AInvadedPlayer* Player = Cast<AInvadedPlayer>(GetOwningPawn());
	if (Player)
	{
		if (Player->GetInvadedStateMachine())
		{
			UOneButtonDialogBox* OneButtonDialogBox = Cast<UOneButtonDialogBox>(Player->GetInvadedStateMachine()->FindStateByName("OneButtonDialogBox"));
			if (OneButtonDialogBox)
			{
				OneButtonDialogBox->Open(Text,MenuStackFlags);
			}
		}
	}
}

TArray<UButton*> APlayerHUD::OpenTwoButtonDialogBox(FText Text, EMenuStackFlags MenuStackFlags)
{
	AInvadedPlayer* Player = Cast<AInvadedPlayer>(GetOwningPawn());
	if (Player)
	{
		if (Player->GetInvadedStateMachine())
		{
			UTwoButtonsDialogBox* TwoButtonsDialogBox = Cast<UTwoButtonsDialogBox>(Player->GetInvadedStateMachine()->FindStateByName("TwoButtonsDialogBox"));
			if (TwoButtonsDialogBox)
			{
				return TwoButtonsDialogBox->Open(Text, MenuStackFlags);
			}
		}
	}
	return TArray<UButton*>();
}
