// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InvadedStateMachine.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API UInvadedStateMachine : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString StartStateID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPushStartState;
public:
	virtual void NativeConstruct() override;
	UFUNCTION(BlueprintCallable, Category = "UserInterface")
		void ToggleState(const FString & StateID);

	UFUNCTION(BlueprintCallable, Category = "UserInterface")
		void ChangeState(const FString& StateID);

	UFUNCTION(BlueprintCallable, Category = "UserInterface")
		void PushState(const FString& StateID);

	UFUNCTION(BlueprintCallable, Category = "UserInterface")
		void AddState(FString StateID);

	UFUNCTION(BlueprintCallable, Category = "UserInterface")
		void RemoveState(FString StateID);

	UFUNCTION(BlueprintCallable, Category = "UserInterface")
		void PopState();

	UFUNCTION(BlueprintCallable, Category = "UserInterface")
		void ClearAllStates();

	UFUNCTION(BlueprintCallable, Category = "UserInterface")
	    class UInvadedMenuState* FindStateByName(const FString& StateID) const;
protected:
	void FindMenuStatesInChildren();

private:
	TArray<class UInvadedMenuState*> States;
	TArray<class UInvadedMenuState*> StateStack;
	TArray<class UInvadedMenuState*> StateArray;

	

	
};
