// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "UObject/ObjectMacros.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Steam/steam_api.h"
#include "GetWorkshopItemInfoProxy.generated.h"

/**
 * 
 */
typedef uint64 PublishedFileId_t;

USTRUCT(BlueprintType)
struct FWorkshopItemDetails
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		FString Title;
	UPROPERTY(BlueprintReadWrite)
		FString Description;
	UPROPERTY(BlueprintReadWrite)
		int32 OwnerID;


	FWorkshopItemDetails()
	{

	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBlueprintGetWorkshopItemInfoDelegate, const FWorkshopItemDetails&, ItemDetails);

UCLASS()
class INVADED_API UGetWorkshopItemInfoProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_BODY()
		// Called when the session was created successfully
	UPROPERTY(BlueprintAssignable)
		FBlueprintGetWorkshopItemInfoDelegate OnSuccess;

	// Called when there was an error creating the session
	UPROPERTY(BlueprintAssignable)
		FBlueprintGetWorkshopItemInfoDelegate OnFailure;

	// Gets workshop item details from steam database
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Steam|Workshop")
		static UGetWorkshopItemInfoProxy* GetWorkshopItemInfo(int32 ItemID);

	// UOnlineBlueprintCallProxyBase interface
	virtual void Activate() override;
	// End of UOnlineBlueprintCallProxyBase interface

	int32 ItemID;

private:
	void OnUGCRequestUGCDetails(SteamUGCRequestUGCDetailsResult_t *pResult, bool bIOFailure);
	CCallResult<UGetWorkshopItemInfoProxy, SteamUGCRequestUGCDetailsResult_t> m_callResultUGCRequestDetails;
};
