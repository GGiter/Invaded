// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/InvadedMenuState.h"
#include "LobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API ULobbyWidget : public UInvadedMenuState
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* StartButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* BackButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* CustomizationButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* LevelSelectionButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* PlayerListButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* StartText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UComboBoxString* BattleTypeComboBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* BattleTypeText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* TimeText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* ReadyPlayersText;

	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent)
		void UpdatePlayerList();

	void LockButtons();
	void SetRemaingTime(const float& Value);
	UFUNCTION()
	void StartClicked();
	UFUNCTION()
	void UpdateTimeText();
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere)
		FLinearColor StartTextColorOn;
	UPROPERTY(EditAnywhere)
		FLinearColor StartTextColorOff;
protected:
	UPROPERTY(BlueprintReadWrite)
		float TimeRemaining;

	bool bPlayerReady=false;

	UFUNCTION(BlueprintCallable)
		void SetIsPlayerReady(bool Value);

	FTimerHandle TimeTimer;
	
};
