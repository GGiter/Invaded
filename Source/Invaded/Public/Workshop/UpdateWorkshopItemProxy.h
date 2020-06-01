// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "Steam/steam_api.h"
#include "InvadedGI.h"
#include "UpdateWorkshopItemProxy.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API UUpdateWorkshopItemProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_BODY()
	// Called when the session was created successfully
	UPROPERTY(BlueprintAssignable)
		FEmptyOnlineDelegate OnSuccess;

	// Called when there was an error creating the session
	UPROPERTY(BlueprintAssignable)
		FEmptyOnlineDelegate OnFailure;

	// Update workshop item
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Steam|Workshop")
		static UUpdateWorkshopItemProxy* UpdateWorkshopItem(int32 ItemID, FWorkshopItem WorkshopItem);

	// UOnlineBlueprintCallProxyBase interface
	virtual void Activate() override;
	// End of UOnlineBlueprintCallProxyBase interface

	int32 ItemID;

	FWorkshopItem WorkshopItem;

private:
	void OnUGCSubmitItemUpdateResult(SubmitItemUpdateResult_t* pResult, bool bIOFailure);
	CCallResult<UUpdateWorkshopItemProxy, SubmitItemUpdateResult_t> m_submitUpdateItemResult_t;
};
