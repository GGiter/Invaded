// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/InvadedMenuState.h"
#include "Online.h"
#include "LeaderboardsWidget.generated.h"

USTRUCT(BlueprintType)
struct FLeaderboardData
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite)
		FString PlayerName;

	UPROPERTY(BlueprintReadWrite)
		int32 Rank;

	UPROPERTY(BlueprintReadWrite)
		FString Score;

	FORCEINLINE bool operator==(const FLeaderboardData& A) const
	{
		return Rank == A.Rank;
	}
};
UENUM(BlueprintType)
enum class ELeaderboardType : uint8
{
	LT_None,
	LT_World,
	LT_Friends
};
UCLASS()
class INVADED_API ULeaderboardsWidget : public UInvadedMenuState
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
		void ReadLeaderboardFriend(const FString& LeaderboardName, const FString& StatName);

	UFUNCTION(BlueprintCallable)
		void ReadLeaderboardsAroundTopRank(const FString& LeaderboardName,const FString& StatName, const int32& Range);

	UFUNCTION(BlueprintPure)
		TArray<FLeaderboardData> GetLeaderboardData(const FString& StatName, ELeaderboardType LeaderboardType);

	UFUNCTION(BlueprintCallable)
		void WriteInteger(const int32& Value);

protected:

	UFUNCTION(BlueprintImplementableEvent)
		void BP_OnReadComplete(ELeaderboardType LeaderboardType);

	FOnlineLeaderboardReadRef LeaderboardReadTopRankData = MakeShared<FOnlineLeaderboardRead, ESPMode::ThreadSafe>();

	FOnlineLeaderboardReadRef LeaderboardReadFriendsData = MakeShared<FOnlineLeaderboardRead, ESPMode::ThreadSafe>();

	ELeaderboardType LastLeaderboardType;

	ELeaderboardType LastReadSuccessfullLeaderboardType = ELeaderboardType::LT_None;

	UFUNCTION()
		void OnReadComplete(bool bWasSuccessfull);
};
