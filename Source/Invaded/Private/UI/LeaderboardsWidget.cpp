// Fill out your copyright notice in the Description page of Project Settings.


#include "LeaderboardsWidget.h"
#include "Engine/World.h"
#include "OnlineLeaderboardInterface.h"
#include "GameFramework/PlayerState.h"

void ULeaderboardsWidget::ReadLeaderboardFriend(const FString& LeaderboardName, const FString& StatName)
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();

	LeaderboardReadFriendsData = MakeShareable(new FOnlineLeaderboardRead());

	LeaderboardReadFriendsData->LeaderboardName = *LeaderboardName;
	LeaderboardReadFriendsData->SortedColumn = *StatName;
	new (LeaderboardReadFriendsData->ColumnMetadata) FColumnMetaData(*StatName, EOnlineKeyValuePairDataType::Int32);

	if (OnlineSubsystem)
	{
		if (OnlineSubsystem->GetLeaderboardsInterface())
		{
			OnlineSubsystem->GetLeaderboardsInterface()->OnLeaderboardReadCompleteDelegates.AddUObject(this, &ULeaderboardsWidget::OnReadComplete);

			OnlineSubsystem->GetLeaderboardsInterface()->ReadLeaderboardsForFriends(0, LeaderboardReadFriendsData);
		}
	}

	LastLeaderboardType = ELeaderboardType::LT_Friends;
}
void ULeaderboardsWidget::ReadLeaderboardsAroundTopRank(const FString & LeaderboardName, const FString& StatName, const int32 & Range)
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();

	LeaderboardReadTopRankData = MakeShareable(new FOnlineLeaderboardRead());

	LeaderboardReadTopRankData->LeaderboardName = *LeaderboardName;
	LeaderboardReadTopRankData->SortedColumn = *StatName;
	new (LeaderboardReadTopRankData->ColumnMetadata) FColumnMetaData(*StatName, EOnlineKeyValuePairDataType::Int32);
	if (OnlineSubsystem)
	{
		if (OnlineSubsystem->GetLeaderboardsInterface())
		{
			OnlineSubsystem->GetLeaderboardsInterface()->OnLeaderboardReadCompleteDelegates.AddUObject(this, &ULeaderboardsWidget::OnReadComplete);

			OnlineSubsystem->GetLeaderboardsInterface()->ReadLeaderboardsAroundRank(1, Range, LeaderboardReadTopRankData);
		}
	}
	
	LastLeaderboardType = ELeaderboardType::LT_World;
}

TArray<FLeaderboardData> ULeaderboardsWidget::GetLeaderboardData(const FString& StatName, ELeaderboardType LeaderboardType)
{
	TArray<FLeaderboardData> RetVal;

	FOnlineLeaderboardRead LeaderboardData;

	if (LeaderboardType == ELeaderboardType::LT_World)
	{
		LeaderboardData = LeaderboardReadTopRankData.Get();
	}
	else
	{
		LeaderboardData = LeaderboardReadFriendsData.Get();
	}

	for (FOnlineStatsRow Row : LeaderboardData.Rows)
	{
		FLeaderboardData Data;
		Data.PlayerName = Row.NickName;
		Data.Rank = Row.Rank;
		if (Row.Columns.Find(*StatName))
		{
			Data.Score = Row.Columns.Find(*StatName)->ToString();
		}

		RetVal.AddUnique(Data);
	}

	return RetVal;
}
void ULeaderboardsWidget::OnReadComplete(bool bWasSuccessfull)
{
	if (bWasSuccessfull)
	{
		if (LastLeaderboardType != LastReadSuccessfullLeaderboardType)
		{
			LastReadSuccessfullLeaderboardType = LastLeaderboardType;
			BP_OnReadComplete(LastLeaderboardType);
		}
		
	}		
}
void ULeaderboardsWidget::WriteInteger(const int32& Value)
{

		APlayerState* PlayerState = GetOwningPlayer()->GetPlayerState<APlayerState>();

		if (PlayerState)
		{
			int32 NewValue = Value;
			TArray<FLeaderboardData> LeaderboardData = GetLeaderboardData("Score",ELeaderboardType::LT_Friends);
			for (FLeaderboardData Row : LeaderboardData)
			{
				if(Row.PlayerName == PlayerState->GetPlayerName())
				{
					NewValue += FCString::Atoi(*Row.Score);
					break;
				}
			}
			FOnlineLeaderboardWrite WriteObject;
			WriteObject.LeaderboardNames.Add("Score");
			WriteObject.RatedStat = "Score";
			WriteObject.DisplayFormat = ELeaderboardFormat::Number;
			WriteObject.SortMethod = ELeaderboardSort::Descending;
			WriteObject.UpdateMethod = ELeaderboardUpdateMethod::Force;
			WriteObject.IncrementIntStat("Score", Value);
			

			TSharedPtr<const FUniqueNetId> UserId = PlayerState->UniqueId.GetUniqueNetId();
			if (UserId.IsValid())
			{
				if (IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::IsLoaded() ? IOnlineSubsystem::Get() : nullptr)
				{
					IOnlineLeaderboardsPtr Leaderboards = OnlineSub->GetLeaderboardsInterface();
					if (Leaderboards.IsValid())
					{
						Leaderboards->WriteLeaderboards(PlayerState->SessionName, *UserId, WriteObject);

						Leaderboards->FlushLeaderboards(PlayerState->SessionName);
					}
				}
			}

		}
	
}