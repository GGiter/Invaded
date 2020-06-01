// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "Steam/steam_api.h"
#include "SubscribeWorkshopItemProxy.generated.h"

/**
 * 
 */
UCLASS()
class INVADED_API USubscribeWorkshopItemProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_BODY()

	// Called when the session was created successfully
	UPROPERTY(BlueprintAssignable)
		FEmptyOnlineDelegate OnSuccess;

	// Called when there was an error creating the session
	UPROPERTY(BlueprintAssignable)
		FEmptyOnlineDelegate OnFailure;

	// Subscribe to workshop item
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Steam|Workshop")
		static USubscribeWorkshopItemProxy* SubscribeWorkshopItem(int32 ItemID);

	// UOnlineBlueprintCallProxyBase interface
	virtual void Activate() override;
	// End of UOnlineBlueprintCallProxyBase interface

	int32 ItemID;

private:

	void OnUGCSubscribeItemResult(RemoteStorageSubscribePublishedFileResult_t* pResult, bool bIOFailure);
	CCallResult<USubscribeWorkshopItemProxy, RemoteStorageSubscribePublishedFileResult_t> m_subscribeItemResult_t;
};
