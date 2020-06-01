// Fill out your copyright notice in the Description page of Project Settings.

#include "InvadedStateMachine.h"
#include "InvadedMenuState.h"
#include "PanelWidget.h"

void UInvadedStateMachine::NativeConstruct()
{
	Super::NativeConstruct();
	FindMenuStatesInChildren();
	if (bPushStartState)
		PushState(StartStateID);
	else
		ChangeState(StartStateID);
}

void UInvadedStateMachine::ToggleState(const FString & StateID)
{
	UInvadedMenuState* NewState = FindStateByName(StateID);
	
	if (NewState)
	{

		if (NewState->GetIsOpen())
		{
			UE_LOG(LogTemp, Warning, TEXT("Remove Dice Pod: %s"), *StateID);
			RemoveState(StateID);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Dice Pod Toggled: %s"), *StateID);
			AddState(StateID);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UInvadedStateMachine could not find menu state with ID: %s"), *StateID)
	}
}

void UInvadedStateMachine::ChangeState(const FString & StateID)
{
	if (StateID.IsEmpty())
		return;

	UInvadedMenuState* NewState = FindStateByName(StateID);
	if (NewState)
	{
		ClearAllStates();
		NewState->OnEnter();
		NewState->OnFocusEnter();
		StateStack.Push(NewState);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UInvadedStateMachine could not find menu state with ID: %s"), *StateID)
	}
}

void UInvadedStateMachine::PushState(const FString & StateID)
{
	if (StateID.IsEmpty())
		return;

	UInvadedMenuState* NewState = FindStateByName(StateID);
	if (NewState)
	{
		switch (NewState->StackFlags)
		{
		case EMenuStackFlags::DrawOnly:
			for (int i = StateStack.Num() - 1; i >= 0; i--)
			{
				if (StateStack[i])
				{
					StateStack[i]->OnFocusExit();
					if (StateStack[i]->StackFlags != EMenuStackFlags::DrawAndUpdate)
						break;
				}
			}
			break;
		case EMenuStackFlags::HideAndDontUpdate:
			for (int i = StateStack.Num() - 1; i >= 0; i--)
			{
				if (StateStack[i])
				{
					StateStack[i]->OnFocusExit();
					if (StateStack[i]->StackFlags != EMenuStackFlags::DrawAndUpdate)
						break;
				}
			}
			for (int i = StateStack.Num() - 1; i >= 0; i--)
			{
				if (StateStack[i])
				{
					StateStack[i]->OnFocusExit();
					if (StateStack[i]->StackFlags != EMenuStackFlags::HideAndDontUpdate)
						break;
				}
			}
			break;
		case EMenuStackFlags::HideOnly:
			for (auto State : StateStack)
			{
				if (State)
				{
					State->OnFocusExit();
					State->OnExit();
				}

			}
			break;
		default:
			break;
		}

		NewState->OnEnter();
		NewState->OnFocusEnter();
		StateStack.Add(NewState);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UInvadedStateMachine could not find menu state with ID: %s"), *StateID)
	}
}
void UInvadedStateMachine::AddState(FString  StateID)
{
	if (StateID.IsEmpty())
		return;

	UInvadedMenuState* NewState = FindStateByName(StateID);
	if (NewState)
	{
		switch (NewState->StackFlags)
		{
		case EMenuStackFlags::DrawOnly:
			for (int i = StateStack.Num() - 1; i >= 0; i--)
			{
				if (StateStack[i])
				{
					StateStack[i]->OnFocusExit();
					if (StateStack[i]->StackFlags != EMenuStackFlags::DrawAndUpdate)
						break;
				}	
			}
			break;
		case EMenuStackFlags::HideAndDontUpdate:
			for (int i = StateStack.Num() - 1; i >= 0; i--)
			{
				if (StateStack[i])
				{
					StateStack[i]->OnFocusExit();
					if (StateStack[i]->StackFlags != EMenuStackFlags::DrawAndUpdate)
						break;
				}
			}
			for (int i = StateStack.Num() - 1; i >= 0; i--)
			{
				if (StateStack[i])
				{
					StateStack[i]->OnFocusExit();
					if (StateStack[i]->StackFlags != EMenuStackFlags::HideAndDontUpdate)
						break;
				}
			}
			break;
		case EMenuStackFlags::HideOnly:
			for (auto State : StateStack)
			{
				if (State)
				{
					State->OnFocusExit();
					State->OnExit();
				}
				
			}
			break;
		default:
			break;
		}
		
		NewState->OnEnter();
		NewState->OnFocusEnter();
		
		StateArray.Add(NewState);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UInvadedStateMachine could not find menu state with ID: %s"), *StateID)
	}
}
void UInvadedStateMachine::RemoveState( FString  StateID)
{
	
		UInvadedMenuState* NewState = FindStateByName(StateID);
		if (NewState)
		{

			NewState->OnFocusExit();
			NewState->OnExit();

			switch (NewState->StackFlags)
			{
			case EMenuStackFlags::DrawOnly:
				for (int i = StateStack.Num() - 1; i >= 0; i--)
				{
					StateStack[i]->OnFocusEnter();
					if (StateStack[i]->StackFlags != EMenuStackFlags::DrawAndUpdate)
						break;
				}
				break;
			case EMenuStackFlags::HideAndDontUpdate:
				for (int i = StateStack.Num() - 1; i >= 0; i--)
				{
					StateStack[i]->OnFocusEnter();
					if (StateStack[i]->StackFlags != EMenuStackFlags::DrawAndUpdate)
						break;
				}
				for (int i = StateStack.Num() - 1; i >= 0; i--)
				{
					StateStack[i]->OnShow();
					if (StateStack[i]->StackFlags == EMenuStackFlags::HideAndDontUpdate)
						break;
				}
				break;
			case EMenuStackFlags::HideOnly:
				for (auto State : StateStack)
				{
					if (State != NewState)
					{
						State->OnFocusEnter();
						State->OnEnter();
					}
					
				}
				break;
			default:
				break;
			}
			StateArray.Remove(NewState);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UInvadedStateMachine could not find menu state with ID: %s"), *StateID)
		}
	
}

void UInvadedStateMachine::PopState()
{
	if (StateStack.Num() > 0)
	{
		UInvadedMenuState* NewState = StateStack.Pop();
		NewState->OnFocusExit();
		NewState->OnExit();

		switch (NewState->StackFlags)
		{
		case EMenuStackFlags::DrawOnly:
			for (int i = StateStack.Num() - 1; i >= 0; i--)
			{
				if (StateStack[i])
				{
					StateStack[i]->OnFocusEnter();
					if (StateStack[i]->StackFlags != EMenuStackFlags::DrawAndUpdate)
						break;
				}
				
			}
			break;
		case EMenuStackFlags::HideAndDontUpdate:
			for (int i = StateStack.Num() - 1; i >= 0; i--)
			{
				if (StateStack[i])
				{
					StateStack[i]->OnFocusEnter();
					if (StateStack[i]->StackFlags != EMenuStackFlags::DrawAndUpdate)
						break;
				}		
			}
			for (int i = StateStack.Num() - 1; i >= 0; i--)
			{
				if (StateStack[i])
				{
					StateStack[i]->OnShow();
					if (StateStack[i]->StackFlags == EMenuStackFlags::HideAndDontUpdate)
						break;
				}	
			}
			break;
		case EMenuStackFlags::HideOnly:
			for (auto State : StateStack)
			{
				if (State)
				{
					State->OnFocusEnter();
					State->OnEnter();
				}		
			}
			for (auto State : StateArray)
			{
				if (State)
				{
					if (State->GetIsOpen())
					{
						State->OnFocusEnter();
						State->OnEnter();
					}
				}		
			}
			break;
		default:
			break;
		}
	}
}

void UInvadedStateMachine::ClearAllStates()
{
	for (auto State : StateStack)
	{
		State->OnExit();
	}

	StateStack.Empty();
}

void UInvadedStateMachine::FindMenuStatesInChildren()
{
	UWidget* RootWidget = GetRootWidget();
	UPanelWidget* RootPanel = RootWidget ? Cast<UPanelWidget>(RootWidget) : nullptr;

	if (RootPanel)
	{
		
		for (int i = 0; i < RootPanel->GetChildrenCount(); i++)
		{
			
			UInvadedMenuState* MenuState = Cast<UInvadedMenuState>(RootPanel->GetChildAt(i));
			if (MenuState)
			{
				MenuState->StateMachine = this;
				States.Add(MenuState);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UInvadedStateMachine doesn't have a root panel!"));
	}
}

UInvadedMenuState * UInvadedStateMachine::FindStateByName(const FString & StateID) const
{
	for (int i = 0; i < States.Num(); i++)
	{
		if (States[i])
		{
			if (States[i]->StateID == StateID)
				return States[i];
		}
	}

	return nullptr;
}
