// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "UnrealNetwork.h"
#include "Online.h"
#include "Invaded.h"
#include "AdvancedFriendsGameInstance.h"
#include "Steam/steam_api.h"
#include "InvadedGI.generated.h"

#define _CRT_SECURE_NO_WARNINGS
/**
 * 
 */
typedef uint64 PublishedFileId_t;

USTRUCT(BlueprintType)
struct FWorkshopItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		FString Title;
	UPROPERTY(BlueprintReadWrite)
		FString Description;
	UPROPERTY(BlueprintReadWrite)
		FString ContentPath;
	UPROPERTY(BlueprintReadWrite)
		FString Changelog;

	FWorkshopItem()
	{

	}
};

UCLASS()
class INVADED_API UInvadedGI : public UAdvancedFriendsGameInstance
{
	GENERATED_BODY()
public:
	virtual void Init() override;
	virtual void Shutdown() override;


	UPROPERTY(EditDefaultsOnly)
	FName MainMenuLevelName="MainMenu";
	UPROPERTY(EditDefaultsOnly)
	FName MainGameLevelName = "MainGame";

	UFUNCTION(BlueprintCallable, Server, WithValidation,Reliable)
		void ServerTravel(const FString& LevelName);
	UFUNCTION(BlueprintCallable)
		void InitPortMapping();
	UFUNCTION(BlueprintCallable)
		void AddPortMapping(int32 Port);
	UFUNCTION(BlueprintCallable)
		void RemovePortMapping(int32 Port);

	UFUNCTION(BlueprintImplementableEvent)
		void ExitLobbyLevel();

	UFUNCTION(BlueprintCallable)
		void ApplyVideoSettings();

	UFUNCTION(BlueprintPure, Category = "Workshop")
		int32 GetSubscribedItemCount();

	UFUNCTION(BlueprintPure, Category = "Workshop")
		TArray<int32> GetSubscribedItemIDs();

	UFUNCTION(BlueprintCallable, Category = "Workshop")
		void SubscribeItem(int32 ItemID);

	UFUNCTION(BlueprintCallable, Category = "Workshop")
		void UnsubscribeItem(int32 ItemID);

	UFUNCTION(BlueprintCallable, Category = "Workshop")
		void GetItemDetails(int32 ItemID);

	UFUNCTION(BlueprintCallable, Category = "Workshop")
		void CreateItem();

	UFUNCTION(BlueprintCallable, Category = "Workshop")
		void UpdateItem(FWorkshopItem WorkshopItem,int32 ItemID);
	
	UFUNCTION(BlueprintCallable, Category = "Workshop")
		void UpdateWorkshopMaps();

	UFUNCTION(BlueprintCallable, Category = "Workshop")
		static TArray<int32> GetInstalledItemIDs() { return UInvadedGI::InstalledItemIDs; }

	static TArray<int32> InstalledItemIDs;
public:
	Portfwd pf;

	WinXPSP2FireWall wf;

	bool bInit;

	PublishedFileId_t CurrentWorkshopItemID;

	void OnUGCCreateItemResult(CreateItemResult_t* pResult, bool bIOFailure);
	CCallResult<UInvadedGI, CreateItemResult_t> m_createItemResult_t;

	void OnUGCSubmitItemUpdateResult(SubmitItemUpdateResult_t* pResult, bool bIOFailure);
	CCallResult<UInvadedGI, SubmitItemUpdateResult_t> m_submitUpdateItemResult_t;

	void OnUGCSubscribeItemResult(RemoteStorageSubscribePublishedFileResult_t* pResult, bool bIOFailure);
	CCallResult<UInvadedGI, RemoteStorageSubscribePublishedFileResult_t> m_subscribeItemResult_t;

	void OnUGCUnsubscribeItemResult(RemoteStorageUnsubscribePublishedFileResult_t* pResult, bool bIOFailure);
	CCallResult<UInvadedGI, RemoteStorageUnsubscribePublishedFileResult_t> m_unsubscribeItemResult_t;

	void OnUGCRequestUGCDetails(SteamUGCRequestUGCDetailsResult_t *pResult, bool bIOFailure);
	CCallResult<UInvadedGI, SteamUGCRequestUGCDetailsResult_t> m_callResultUGCRequestDetails;

	UPROPERTY(BlueprintReadWrite, Category = "Workshop")
	bool bItemCreated = false;
	
};
