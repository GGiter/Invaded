// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/DialogBox.h"
#include "OneButtonDialogBox.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API UOneButtonDialogBox : public UDialogBox
{
	GENERATED_BODY()
public:
	
	void Open(FText Content, EMenuStackFlags MenuStackFlags);
	UFUNCTION(BlueprintCallable)
	void Close();
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* ContentText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* OkayButton;
};
