// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "UObject/ObjectMacros.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Steam/steam_api.h"
#include "CreateWorkshopItemProxy.generated.h"

/**
 * 
 */
typedef uint64 PublishedFileId_t;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBlueprintCreateWorkshopItemResultDelegate, const int32&, ItemID);

UCLASS()
class INVADED_API UCreateWorkshopItemProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_BODY()

		// Called when the session was created successfully
	UPROPERTY(BlueprintAssignable)
		FBlueprintCreateWorkshopItemResultDelegate OnSuccess;

	// Called when there was an error creating the session
	UPROPERTY(BlueprintAssignable)
		FBlueprintCreateWorkshopItemResultDelegate OnFailure;

	// Creates workshop item id
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Steam|Workshop")
		static UCreateWorkshopItemProxy* CreateWorkshopItem();

	// UOnlineBlueprintCallProxyBase interface
	virtual void Activate() override;
	// End of UOnlineBlueprintCallProxyBase interface

private:
	void OnUGCCreateItemResult(CreateItemResult_t* pResult, bool bIOFailure);
	CCallResult<UCreateWorkshopItemProxy, CreateItemResult_t> m_createItemResult_t;


};
