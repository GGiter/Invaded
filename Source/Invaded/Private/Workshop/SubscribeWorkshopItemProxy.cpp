// Fill out your copyright notice in the Description page of Project Settings.


#include "SubscribeWorkshopItemProxy.h"
#include "Async/Async.h"

USubscribeWorkshopItemProxy * USubscribeWorkshopItemProxy::SubscribeWorkshopItem(int32 ItemID)
{
	USubscribeWorkshopItemProxy* Proxy = NewObject<USubscribeWorkshopItemProxy>();
	Proxy->ItemID = ItemID;
	return Proxy;
}

void USubscribeWorkshopItemProxy::Activate()
{
	if (SteamAPI_Init())
	{
		SteamAPICall_t hSteamAPICall = SteamUGC()->SubscribeItem(ItemID);
		m_subscribeItemResult_t.Set(hSteamAPICall, this, &USubscribeWorkshopItemProxy::OnUGCSubscribeItemResult);
	}
}

void USubscribeWorkshopItemProxy::OnUGCSubscribeItemResult(RemoteStorageSubscribePublishedFileResult_t * pResult, bool bIOFailure)
{
	if (IsInGameThread())
	{
		if (bIOFailure)
		{
			OnFailure.Broadcast();
			return;
		}

		OnSuccess.Broadcast();
	}
	else
	{
		AsyncTask(ENamedThreads::GameThread, [&] {

			if (bIOFailure)
			{
				OnFailure.Broadcast();
				return;
			}

			OnSuccess.Broadcast();
		});
	}
	
}
