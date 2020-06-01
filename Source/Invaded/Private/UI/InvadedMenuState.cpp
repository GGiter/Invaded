// Fill out your copyright notice in the Description page of Project Settings.

#include "InvadedMenuState.h"


void UInvadedMenuState::NativeConstruct()
{
	Super::NativeConstruct();
	bHasFocusInHierarchy = false;
	bHasFocusSelf = true;
	bVisibleInHierarchy = false;
	bVisibleSelf = true;
	bIsOpen = false;
}

void UInvadedMenuState::OnEnter()
{
	
	OnShow();
	bIsOpen = true;
	ReceiveOnEnter();
}

void UInvadedMenuState::OnExit()
{
	OnHide();
	bIsOpen = false;
	ReceiveOnExit();
}

void UInvadedMenuState::OnFocusEnter()
{
	SetHasFocusInHierarchy(true);
}

void UInvadedMenuState::OnFocusExit()
{
	SetHasFocusInHierarchy(false);
}

void UInvadedMenuState::OnShow()
{
	SetVisibleInHierarchy(true);
}

void UInvadedMenuState::OnHide()
{
	SetVisibleInHierarchy(false);
}

void UInvadedMenuState::SetIsOpen(const bool & Value)
{
	bIsOpen = Value;
}

void UInvadedMenuState::SetVisibleInHierarchy(const bool & Value)
{
	bVisibleInHierarchy = Value;
	UpdateVisibility();
}

void UInvadedMenuState::SetHasFocusInHierarchy(const bool & Value)
{
	bHasFocusInHierarchy = Value;
	UpdateFocus();
}

void UInvadedMenuState::SetVisibleSelf(const bool & Value)
{
	bVisibleSelf = Value;
	UpdateVisibility();
}

void UInvadedMenuState::SetHasSelfFocus(const bool & Value)
{
	bHasFocusSelf = Value;
	UpdateFocus();
}

void UInvadedMenuState::UpdateVisibility()
{
	if (bVisibleInHierarchy && bVisibleSelf)
	{
		SetVisibility(bHasFocusInHierarchy && bHasFocusSelf ? ESlateVisibility::Visible : ESlateVisibility::HitTestInvisible);
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UInvadedMenuState::UpdateFocus()
{
	if (bIsOpen && bVisibleInHierarchy && bVisibleSelf)
	{
		SetVisibility(bHasFocusInHierarchy && bHasFocusSelf ? ESlateVisibility::Visible : ESlateVisibility::HitTestInvisible);
	}
}
FSlateColor UInvadedMenuState::GetTextColor()
{
	if (PlayerMaterialCollection)
	{
		return UKismetMaterialLibrary::GetVectorParameterValue(GetWorld(), PlayerMaterialCollection, "P_Color1");
	}
	else
	{
		return FSlateColor();
	}
}