// Fill out your copyright notice in the Description page of Project Settings.


#include "UpdateWorkshopItemProxy.h"

UUpdateWorkshopItemProxy* UUpdateWorkshopItemProxy::UpdateWorkshopItem(int32 ItemID, FWorkshopItem WorkshopItem)
{
	UUpdateWorkshopItemProxy* Proxy = NewObject<UUpdateWorkshopItemProxy>();
	Proxy->ItemID = ItemID;
	Proxy->WorkshopItem = WorkshopItem;
	return Proxy;
}

void UUpdateWorkshopItemProxy::Activate()
{
	if (SteamAPI_Init() && WorkshopItem.ContentPath != "")
	{
		UGCUpdateHandle_t UpdateHandle = SteamUGC()->StartItemUpdate(1073780, ItemID);

		SteamUGC()->SetItemTitle(UpdateHandle, TCHAR_TO_ANSI(*WorkshopItem.Title));

		SteamUGC()->SetItemContent(UpdateHandle, TCHAR_TO_ANSI(*WorkshopItem.ContentPath));

		SteamUGC()->SetItemVisibility(UpdateHandle, ERemoteStoragePublishedFileVisibility::k_ERemoteStoragePublishedFileVisibilityPublic);

		SteamUGC()->SetItemDescription(UpdateHandle, TCHAR_TO_ANSI(*WorkshopItem.Description));

		SteamUGC()->SetItemUpdateLanguage(UpdateHandle, "English");

		SteamAPICall_t hSteamAPICall = SteamUGC()->SubmitItemUpdate(UpdateHandle, TCHAR_TO_ANSI(*WorkshopItem.Changelog));
		m_submitUpdateItemResult_t.Set(hSteamAPICall, this, &UUpdateWorkshopItemProxy::OnUGCSubmitItemUpdateResult);
	}
}

void UUpdateWorkshopItemProxy::OnUGCSubmitItemUpdateResult(SubmitItemUpdateResult_t * pResult, bool bIOFailure)
{
	if (bIOFailure)
	{
		OnFailure.Broadcast();
		return;
	}
	OnSuccess.Broadcast();
}
