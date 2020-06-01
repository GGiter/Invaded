// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "Steam/steam_api.h"
#include "UnsubscribeWorkshopItemProxy.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API UUnsubscribeWorkshopItemProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_BODY()

	// Called when the session was created successfully
	UPROPERTY(BlueprintAssignable)
		FEmptyOnlineDelegate OnSuccess;

	// Called when there was an error creating the session
	UPROPERTY(BlueprintAssignable)
		FEmptyOnlineDelegate OnFailure;

	// Unsubscribe to workshop item
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Steam|Workshop")
		static UUnsubscribeWorkshopItemProxy* UnsubscribeWorkshopItem(int32 ItemID);

	// UOnlineBlueprintCallProxyBase interface
	virtual void Activate() override;
	// End of UOnlineBlueprintCallProxyBase interface

	int32 ItemID;

private:

	void OnUGCUnsubscribeItemResult(RemoteStorageUnsubscribePublishedFileResult_t* pResult, bool bIOFailure);
	CCallResult<UUnsubscribeWorkshopItemProxy, RemoteStorageUnsubscribePublishedFileResult_t> m_unsubscribeItemResult_t;
};
