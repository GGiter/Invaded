// Fill out your copyright notice in the Description page of Project Settings.


#include "InvadedUtilityFunctionLibrary.h"

bool UInvadedUtilityFunctionLibrary::IsWithEditor()
{
#if WITH_EDITOR
	return true;
#else
	return false;
#endif
}