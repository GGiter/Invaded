// Fill out your copyright notice in the Description page of Project Settings.


#include "UnsubscribeWorkshopItemProxy.h"
#include "Async/Async.h"

UUnsubscribeWorkshopItemProxy * UUnsubscribeWorkshopItemProxy::UnsubscribeWorkshopItem(int32 ItemID)
{
	UUnsubscribeWorkshopItemProxy* Proxy = NewObject<UUnsubscribeWorkshopItemProxy>();
	Proxy->ItemID = ItemID;
	return Proxy;
}

void UUnsubscribeWorkshopItemProxy::Activate()
{
	if (SteamAPI_Init())
	{
		SteamAPICall_t hSteamAPICall = SteamUGC()->UnsubscribeItem(ItemID);
		m_unsubscribeItemResult_t.Set(hSteamAPICall, this, &UUnsubscribeWorkshopItemProxy::OnUGCUnsubscribeItemResult);
	}
}

void UUnsubscribeWorkshopItemProxy::OnUGCUnsubscribeItemResult(RemoteStorageUnsubscribePublishedFileResult_t * pResult, bool bIOFailure)
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
