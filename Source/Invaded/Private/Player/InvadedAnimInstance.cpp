// Fill out your copyright notice in the Description page of Project Settings.

#include "InvadedAnimInstance.h"
#include "UnrealNetwork.h"
#include "InvadedPlayer.h"
#include "InvadedPC.h"
#include "InvadedData.h"
#include "InvadedAnimInstance.h"

UInvadedAnimInstance::UInvadedAnimInstance()
{
	
}
void UInvadedAnimInstance::SetMoveStatus(EMoveStatus Value)
{
	MoveStatus = Value;
}
void UInvadedAnimInstance::PlayHitMontage()
{
	BP_PlayHitMontage();
}
void UInvadedAnimInstance::PlayDeathMontage()
{
	BP_PlayDeathMontage();
}
void UInvadedAnimInstance::PlayWinMontage()
{
	BP_PlayWinMontage();
}
void UInvadedAnimInstance::SetIsHit(const bool & Value)
{
	bHit = Value;
}

void UInvadedAnimInstance::SetIsShooting(const bool & Value)
{
	bShooting = Value;
}

void UInvadedAnimInstance::SetHaveWon(const bool & Value)
{
	bWon = Value;
}

void UInvadedAnimInstance::SetIsDead(const bool & Value)
{
	bDead = Value;
}

void UInvadedAnimInstance::SetTurnMode(ETurnMode  Mode)
{
	TurnMode = Mode;
	Execute_PlayTurnMontage(this);
}
