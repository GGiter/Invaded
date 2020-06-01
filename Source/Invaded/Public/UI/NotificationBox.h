// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/InvadedMenuState.h"
#include "NotificationBox.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API UNotificationBox : public UInvadedMenuState
{
	GENERATED_BODY()
public:
	
	void Open(FText Content, float Time);
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* ContentText;
	UPROPERTY(BlueprintReadOnly)
		float DisplayTime;
};
