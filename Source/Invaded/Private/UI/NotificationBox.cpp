// Fill out your copyright notice in the Description page of Project Settings.


#include "NotificationBox.h"
#include "TextBlock.h"
#include "InvadedStateMachine.h"

void UNotificationBox::Open(FText Content, float Time)
{
	if (ContentText)
	{
		ContentText->SetText(Content);
	}

	DisplayTime = Time;

	if (StateMachine)
	{
		StateMachine->AddState(StateID);
	}
}