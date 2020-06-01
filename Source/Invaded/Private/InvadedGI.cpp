// Fill out your copyright notice in the Description page of Project Settings.

#include "InvadedGI.h"
#include "Kismet/GameplayStatics.h"
#include "Engine.h"
#include "..\Public\InvadedGI.h"
#include "VideoSettings.h"
#include "SoundSettings.h"
#include "PlayerSettings.h"	
#include "HAL/FileManager.h"
#include <fstream>

TArray<int32> UInvadedGI::InstalledItemIDs = TArray<int32>();
void UInvadedGI::Init()
{
	Super::Init();
	UVideoSettings::LoadSettings();

	UPlayerSettings::LoadSettings();

	UVideoSettings::Apply(GEngine);

	USoundSettings::LoadSettings();


	wf.Initialize();

	wf.AddPort(7777, NET_FW_IP_PROTOCOL_TCP, L"ABC");
	wf.AddPort(7777, NET_FW_IP_PROTOCOL_UDP, L"ABC");
	wf.AddPort(27015, NET_FW_IP_PROTOCOL_TCP, L"ABC");
	wf.AddPort(27015, NET_FW_IP_PROTOCOL_UDP, L"ABC");
	wf.AddPort(7778, NET_FW_IP_PROTOCOL_TCP, L"ABC");
	wf.AddPort(7778, NET_FW_IP_PROTOCOL_UDP, L"ABC");

	UpdateWorkshopMaps();
}
void UInvadedGI::Shutdown()
{

	wf.RemovePort(7777, NET_FW_IP_PROTOCOL_TCP);
	wf.RemovePort(7777, NET_FW_IP_PROTOCOL_UDP);
	wf.RemovePort(27015, NET_FW_IP_PROTOCOL_TCP);
	wf.RemovePort(27015, NET_FW_IP_PROTOCOL_UDP);
	wf.RemovePort(7778, NET_FW_IP_PROTOCOL_TCP);
	wf.RemovePort(7778, NET_FW_IP_PROTOCOL_UDP);

	Super::Shutdown();


	UPlayerSettings::SaveSettings();
	UVideoSettings::SaveSettings();
	USoundSettings::SaveSettings();
}
void UInvadedGI::ServerTravel_Implementation(const FString& LevelName)
{
	GetWorld()->ServerTravel(LevelName);
}
bool UInvadedGI::ServerTravel_Validate(const FString& LevelName)
{
	return true;
}
void UInvadedGI::InitPortMapping()
{
	if (pf.init(2000))
		bInit = true;
	else
		bInit = false;
}
void UInvadedGI::AddPortMapping(int32 Port)
{
	if (bInit)
	{
		if (!pf.add(Port))
		{
			UE_LOG(LogTemp, Warning, TEXT("Adding port fail"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Adding port success"));
		}
	}
	
		
}
void UInvadedGI::RemovePortMapping(int32 Port)
{
	if (bInit)
	{
		pf.remove(Port);
	}
}
void UInvadedGI::ApplyVideoSettings()
{
	UVideoSettings::Apply(GEngine);
}
void copyFile_internal(const char* src_loc, const char* dest_loc)
{
	std::fstream in, out;
	in.open(src_loc, std::fstream::in | std::fstream::binary);

	if (in.is_open())
	{
		out.open(dest_loc, std::fstream::out);

		char tmp;
		while (in.read(&tmp, 1))
		{
			out.write(&tmp, 1);
		}

		out.close();
	}

	in.close();

}
bool copyFile(FString SRC, FString DEST)
{
	FJsonSerializableArray Files;
	IFileManager::Get().FindFiles(Files, *SRC);
	if (Files.Num() == 0)
		return false;

	for (FString File : Files)
	{
		copyFile_internal(TCHAR_TO_ANSI(*(SRC+"\\"+File)), TCHAR_TO_ANSI(*(DEST + "\\" + File)));
	}

	return true;
}


void UInvadedGI::UpdateWorkshopMaps()
{
	
	FString RelativePath = FPaths::GameContentDir();

	FString FullPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*RelativePath);

	FullPath += "GridMaps";
	for (PublishedFileId_t id : GetSubscribedItemIDs())
	{
		if (SteamAPI_Init())
		{
			char filepath[1024] = { 0 };
			uint64 Size=0;
			uint32 TimeStamp = 0;
			uint32 unItemState = SteamUGC()->GetItemState(id);
			if (!(unItemState & k_EItemStateInstalled))
			{
				UE_LOG(LogTemp, Warning, TEXT("Workshop item id :%d not installed."), id)
				
			}
			if (SteamUGC()->GetItemInstallInfo(id, &Size, filepath, sizeof(filepath), &TimeStamp))
			{
				if (copyFile(ANSI_TO_TCHAR(filepath), FullPath))
				{
					UE_LOG(LogTemp, Log, TEXT("Installed workshop item id :%d."), id)
					UInvadedGI::InstalledItemIDs.Add(id);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Failed to copy workshop item id :%d. %s %s"), id, ANSI_TO_TCHAR(filepath),*FullPath)
				}

			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to get workshop item id :%d install info %s."), id, ANSI_TO_TCHAR(filepath));
			}
			
		}
	}

	
}
void UInvadedGI::GetItemDetails(int32 ItemID)
{
	if (SteamAPI_Init())
	{
		SteamAPICall_t hSteamAPICall = SteamUGC()->RequestUGCDetails(ItemID, 20);
		m_callResultUGCRequestDetails.Set(hSteamAPICall, this, &UInvadedGI::OnUGCRequestUGCDetails);
	}
	
}
int32 UInvadedGI::GetSubscribedItemCount()
{
	if (SteamAPI_Init())
	{
		return SteamUGC()->GetNumSubscribedItems();
	}
	else
	{
		return 0;
	}
}

TArray<int32> UInvadedGI::GetSubscribedItemIDs()
{
	TArray<int32> outArray;

	if (SteamAPI_Init())
	{
		PublishedFileId_t fileIds[16];
		int32 subItems = SteamUGC()->GetSubscribedItems(fileIds, 16);

		for (int i = 0; i < subItems; i++)
		{
			PublishedFileId_t id = fileIds[i];
			outArray.Add(id);
		}
	}
	else
	{
		outArray.Add(0);
	}
	return outArray;
}

void UInvadedGI::SubscribeItem(int32 ItemID)
{
	if (SteamAPI_Init())
	{
		SteamAPICall_t hSteamAPICall = SteamUGC()->SubscribeItem(ItemID);
		m_subscribeItemResult_t.Set(hSteamAPICall, this, &UInvadedGI::OnUGCSubscribeItemResult);
	}
}

void UInvadedGI::UnsubscribeItem(int32 ItemID)
{	
	if (SteamAPI_Init())
	{
		SteamAPICall_t hSteamAPICall = SteamUGC()->UnsubscribeItem(ItemID);
		m_unsubscribeItemResult_t.Set(hSteamAPICall, this, &UInvadedGI::OnUGCUnsubscribeItemResult);
	}	
}
void UInvadedGI::UpdateItem(FWorkshopItem WorkshopItem,int32 ItemID)
{

	if (SteamAPI_Init()&& WorkshopItem.ContentPath != "")
	{
		UGCUpdateHandle_t UpdateHandle = SteamUGC()->StartItemUpdate(1073780, ItemID);

		SteamUGC()->SetItemTitle(UpdateHandle, TCHAR_TO_ANSI(*WorkshopItem.Title));

		SteamUGC()->SetItemContent(UpdateHandle, TCHAR_TO_ANSI(*WorkshopItem.ContentPath));

		SteamUGC()->SetItemVisibility(UpdateHandle, ERemoteStoragePublishedFileVisibility::k_ERemoteStoragePublishedFileVisibilityPublic);


		SteamUGC()->SetItemDescription(UpdateHandle, TCHAR_TO_ANSI(*WorkshopItem.Description));
	
		SteamUGC()->SetItemUpdateLanguage(UpdateHandle, "English");

		SteamAPICall_t hSteamAPICall = SteamUGC()->SubmitItemUpdate(UpdateHandle, TCHAR_TO_ANSI(*WorkshopItem.Changelog));
		m_submitUpdateItemResult_t.Set(hSteamAPICall, this, &UInvadedGI::OnUGCSubmitItemUpdateResult);
	}

}
void UInvadedGI::CreateItem()
{
	
	if (SteamAPI_Init())
	{
		bItemCreated = false;
		SteamAPICall_t hSteamAPICall = SteamUGC()->CreateItem(1073780, EWorkshopFileType::k_EWorkshopFileTypeCommunity);
		m_createItemResult_t.Set(hSteamAPICall, this, &UInvadedGI::OnUGCCreateItemResult);
	}
	
}

void UInvadedGI::OnUGCCreateItemResult(CreateItemResult_t * pResult, bool bIOFailure)
{
	if (bIOFailure)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create item"));
		return;
	}
	
	UE_LOG(LogTemp, Log, TEXT("Created item %d"), CurrentWorkshopItemID);

	bItemCreated = true;

	CurrentWorkshopItemID = pResult->m_nPublishedFileId;
	
}

void UInvadedGI::OnUGCSubmitItemUpdateResult(SubmitItemUpdateResult_t * pResult, bool bIOFailure)
{
	if (bIOFailure)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to submit item"));
		return;
	}

	UE_LOG(LogTemp,Log, TEXT("Submitted item"));
}

void UInvadedGI::OnUGCSubscribeItemResult(RemoteStorageSubscribePublishedFileResult_t * pResult, bool bIOFailure)
{
	if (bIOFailure)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to subscribe item"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Subscribed item"));
}

void UInvadedGI::OnUGCUnsubscribeItemResult(RemoteStorageUnsubscribePublishedFileResult_t * pResult, bool bIOFailure)
{
	if (bIOFailure)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to unsubscribe item"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Unsubscribed item"));
}
void UInvadedGI::OnUGCRequestUGCDetails(SteamUGCRequestUGCDetailsResult_t *pResult, bool bIOFailure)
{
	if (bIOFailure)
	{
		return;
	}
}

