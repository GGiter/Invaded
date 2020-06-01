// Fill out your copyright notice in the Description page of Project Settings.


#include "OneButtonDialogBox.h"
#include "TextBlock.h"
#include "Button.h"
#include "InvadedStateMachine.h"

void UOneButtonDialogBox::Open(FText Content,EMenuStackFlags MenuStackFlags)
{
	if (ContentText)
	{
		ContentText->SetText(Content);
	}


	if (OkayButton)
	{
		OkayButton->OnClicked.AddDynamic(this, &UOneButtonDialogBox::Close);
	}
	

	if (StateMachine)
	{
		StackFlags = MenuStackFlags;
		StateMachine->AddState(StateID);
	}
}

void UOneButtonDialogBox::Close()
{
	if (OkayButton)
	{
		OkayButton->OnClicked.Clear();
	}

	if (StateMachine)
	{
		StateMachine->RemoveState(StateID);
	}
}
