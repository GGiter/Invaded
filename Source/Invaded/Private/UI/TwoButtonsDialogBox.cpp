// Fill out your copyright notice in the Description page of Project Settings.


#include "TwoButtonsDialogBox.h"
#include "TextBlock.h"
#include "InvadedStateMachine.h"



TArray<UButton*> UTwoButtonsDialogBox::Open(FText Content, EMenuStackFlags MenuStackFlags)
{
	if (ContentText)
	{
		ContentText->SetText(Content);
	}


	if (AcceptButton)
	{
		AcceptButton->OnClicked.AddDynamic(this, &UTwoButtonsDialogBox::Close);
	}

	if (DeclineButton)
	{
		DeclineButton->OnClicked.AddDynamic(this, &UTwoButtonsDialogBox::Close);
	}


	if (StateMachine)
	{
		StackFlags = MenuStackFlags;
		StateMachine->AddState(StateID);
	}

	return TArray < UButton* > {DeclineButton, AcceptButton};
}
TArray<UButton*> UTwoButtonsDialogBox::Open_BP(FText Content, EMenuStackFlags MenuStackFlags)
{
	return Open(Content, MenuStackFlags);
}
void UTwoButtonsDialogBox::Close()
{
	if (AcceptButton)
	{
		AcceptButton->OnClicked.Clear();
	}

	if (DeclineButton)
	{
		DeclineButton->OnClicked.Clear();
	}

	if (StateMachine)
	{
		StateMachine->RemoveState(StateID);
	}
}
