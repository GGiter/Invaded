// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/DialogBox.h"
#include "Button.h"
#include "TwoButtonsDialogBox.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API UTwoButtonsDialogBox : public UDialogBox
{
	GENERATED_BODY()
public:
	
	TArray<UButton*> Open(FText Content, EMenuStackFlags MenuStackFlags);
	UFUNCTION(BlueprintCallable)
	void Close();

	UFUNCTION(BlueprintCallable)
	TArray<UButton*> Open_BP(FText Content, EMenuStackFlags MenuStackFlags);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* ContentText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* AcceptButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* DeclineButton;
	
};
