// Fill out your copyright notice in the Description page of Project Settings.


#include "GetWorkshopItemInfoProxy.h"
#include "Async/Async.h"

UGetWorkshopItemInfoProxy* UGetWorkshopItemInfoProxy::GetWorkshopItemInfo(int32 ItemID)
{
	UGetWorkshopItemInfoProxy* Proxy = NewObject<UGetWorkshopItemInfoProxy>();
	Proxy->ItemID = ItemID;
	return Proxy;
}

void UGetWorkshopItemInfoProxy::Activate()
{
	if (SteamAPI_Init())
	{
		SteamAPICall_t hSteamAPICall = SteamUGC()->RequestUGCDetails(ItemID, 20);
		m_callResultUGCRequestDetails.Set(hSteamAPICall, this, &UGetWorkshopItemInfoProxy::OnUGCRequestUGCDetails);
	}
}

void UGetWorkshopItemInfoProxy::OnUGCRequestUGCDetails(SteamUGCRequestUGCDetailsResult_t * pResult, bool bIOFailure)
{
	AsyncTask(ENamedThreads::GameThread, [=]()
	{
		if (bIOFailure)
		{
			OnFailure.Broadcast(FWorkshopItemDetails());
			return;
		}

		FWorkshopItemDetails WorkshopItemDetails;
		WorkshopItemDetails.Title = pResult->m_details.m_pchFileName;
		WorkshopItemDetails.Description = pResult->m_details.m_rgchDescription;
		WorkshopItemDetails.OwnerID = pResult->m_details.m_ulSteamIDOwner;
		OnSuccess.Broadcast(WorkshopItemDetails);
	});
}
