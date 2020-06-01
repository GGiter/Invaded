// Fill out your copyright notice in the Description page of Project Settings.


#include "CreateWorkshopItemProxy.h"
#include "EngineGlobals.h"
#include "Engine/Engine.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"



UCreateWorkshopItemProxy * UCreateWorkshopItemProxy::CreateWorkshopItem()
{
	UCreateWorkshopItemProxy* Proxy = NewObject<UCreateWorkshopItemProxy>();
	return Proxy;
}

void UCreateWorkshopItemProxy::Activate()
{
	if (SteamAPI_Init())
	{
		SteamAPICall_t hSteamAPICall = SteamUGC()->CreateItem(1073780, EWorkshopFileType::k_EWorkshopFileTypeCommunity);
		m_createItemResult_t.Set(hSteamAPICall, this, &UCreateWorkshopItemProxy::OnUGCCreateItemResult);
	}
}

void UCreateWorkshopItemProxy::OnUGCCreateItemResult(CreateItemResult_t * pResult, bool bIOFailure)
{
	if (bIOFailure)
	{
		OnFailure.Broadcast(0);
		return;
	}
	OnSuccess.Broadcast(pResult->m_nPublishedFileId);
}
