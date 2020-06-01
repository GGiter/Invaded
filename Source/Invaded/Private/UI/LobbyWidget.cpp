// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyWidget.h"
#include "Button.h"
#include "TextBlock.h"
#include "Player/LobbyPC.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "ComboBoxString.h"
#include "InvadedStateMachine.h"


void ULobbyWidget::LockButtons()
{
	if(StartButton)
	StartButton->SetVisibility(ESlateVisibility::Hidden);
	if(BackButton)
	BackButton->SetVisibility(ESlateVisibility::Hidden);
	if(ReadyPlayersText)
	ReadyPlayersText->SetVisibility(ESlateVisibility::Hidden);
	if (LevelSelectionButton)
	LevelSelectionButton->SetVisibility(ESlateVisibility::Hidden);
	if (CustomizationButton)
	CustomizationButton->SetVisibility(ESlateVisibility::Hidden);
	if (PlayerListButton)
	PlayerListButton->SetVisibility(ESlateVisibility::Hidden);
	if (BattleTypeComboBox)
	BattleTypeComboBox->SetVisibility(ESlateVisibility::Hidden);
	if (BattleTypeText)
	BattleTypeText->SetVisibility(ESlateVisibility::Hidden);
}

void ULobbyWidget::SetRemaingTime(const float & Value)
{
	TimeRemaining = Value;
	if(TimeText)
	TimeText->SetVisibility(ESlateVisibility::Visible);
	if(TimeText)
	TimeText->SetText(FText::FromString(FString::FromInt(TimeRemaining)));
	GetWorld()->GetTimerManager().SetTimer(TimeTimer, this, &ULobbyWidget::UpdateTimeText, 1.0f, true);
}

void ULobbyWidget::StartClicked()
{
	SetIsPlayerReady(true);
}

void ULobbyWidget::UpdateTimeText()
{
	TimeRemaining--;
	if (TimeRemaining > -1)
	{
		if(TimeText)
		TimeText->SetText(FText::FromString(FString::FromInt(TimeRemaining)));
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(TimeTimer);

		if(StateMachine)
		StateMachine->RemoveFromParent();
	}
		
}

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (StartButton)
		StartButton->OnClicked.AddDynamic(this, &ULobbyWidget::StartClicked);
}

void ULobbyWidget::SetIsPlayerReady(bool Value)
{
	ALobbyPC* LobbyPC = Cast<ALobbyPC>(GetOwningPlayer());
	if (LobbyPC)
	{
		bPlayerReady = Value;
		LobbyPC->SetPlayerReady(bPlayerReady);

		if (bPlayerReady)
		{
			if (StartButton)
				StartButton->SetVisibility(ESlateVisibility::Hidden);
			if (BackButton)
				BackButton->SetVisibility(ESlateVisibility::Hidden);
		//	if (ReadyPlayersText)
			//	ReadyPlayersText->SetVisibility(ESlateVisibility::Hidden);
			if (LevelSelectionButton)
				LevelSelectionButton->SetVisibility(ESlateVisibility::Hidden);
			if (CustomizationButton)
				CustomizationButton->SetVisibility(ESlateVisibility::Hidden);
			if (PlayerListButton)
				PlayerListButton->SetVisibility(ESlateVisibility::Hidden);
			if (BattleTypeComboBox)
				BattleTypeComboBox->SetVisibility(ESlateVisibility::Hidden);
			if (BattleTypeText)
				BattleTypeText->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			if (StartButton)
				StartButton->SetVisibility(ESlateVisibility::Visible);
			if (BackButton)
				BackButton->SetVisibility(ESlateVisibility::Visible);
		//	if (ReadyPlayersText)
		//		ReadyPlayersText->SetVisibility(ESlateVisibility::Visible);
			if (LevelSelectionButton)
				LevelSelectionButton->SetVisibility(ESlateVisibility::Visible);
			if (CustomizationButton)
				CustomizationButton->SetVisibility(ESlateVisibility::Visible);
			if (PlayerListButton)
				PlayerListButton->SetVisibility(ESlateVisibility::Visible);
		//	if (BattleTypeComboBox)
		//		BattleTypeComboBox->SetVisibility(ESlateVisibility::Visible);
		//	if (BattleTypeText)
			//	BattleTypeText->SetVisibility(ESlateVisibility::Visible);
		}
	}
}
