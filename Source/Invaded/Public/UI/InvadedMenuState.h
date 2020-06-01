// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "InvadedMenuState.generated.h"

UENUM(BlueprintType)
enum class EMenuStackFlags : uint8
{
	DrawAndUpdate,
	DrawOnly,
	HideAndDontUpdate,
	HideOnly
};

UCLASS()
class INVADED_API UInvadedMenuState : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString StateID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMenuStackFlags StackFlags;

	UPROPERTY(BlueprintReadWrite)
	class UInvadedStateMachine* StateMachine;

public:

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable, meta = (DisplayName = "OnEnter"))
		void ReceiveOnEnter();

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable, meta = (DisplayName = "OnExit"))
		void ReceiveOnExit();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Open"))
		void Open();
	
	UFUNCTION(BlueprintPure, Category = "Color")
		FSlateColor GetTextColor();

	virtual void OnEnter();
	virtual void OnExit();
	virtual void OnFocusEnter();
	virtual void OnFocusExit();
	void OnShow();
	void OnHide();
	void SetIsOpen(const bool& Value);
	bool GetIsOpen() const { return bIsOpen; }
protected:
	virtual void NativeConstruct() override;
	
	void SetVisibleInHierarchy(const bool& Value);
	void SetHasFocusInHierarchy(const bool& Value);
	void SetVisibleSelf(const bool& Value);
	void SetHasSelfFocus(const bool& Value);
	void UpdateVisibility();
	void UpdateFocus();

	UPROPERTY(EditAnywhere, Category = "Colors")
		UMaterialParameterCollection* PlayerMaterialCollection;
protected:
	bool bIsOpen;
private:
	bool bVisibleSelf;
	bool bVisibleInHierarchy;
	bool bHasFocusInHierarchy;
	bool bHasFocusSelf;
};
