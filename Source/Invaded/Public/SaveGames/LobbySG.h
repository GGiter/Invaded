// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LobbySG.generated.h"

/**
 * 
 */
template <typename T,typename X>
struct FMapValue
{
public:
	T key;
	X value;
	FMapValue()
	{
	}
	FMapValue(T Key, X Value)
	{
		this->key = Key;
		this->value = Value;
	}
};
template <typename T, typename X>
struct FMap
{
public:
	TArray<FMapValue<T, X>> Array;

	int32 Find(T Key)
	{
		int32 index = -1;
		for (int32 i = 0; i < Array.Num(); ++i)
		{
			if (Array[i].key == Key)
			{
				index = i;
				break;
			}
		}
		return index;
	}
	void Add(T Key, X Value)
	{
		Array.Add(FMapValue<T, X>(Key, Value));
	}
	X operator[] (T Key)
	{
		X Value;
		for (int32 i = 0; i < Array.Num(); ++i)
		{
			if (Array[i].key == Key)
			{
				Value=Array[i].value;
				break;
			}
		}
		return Value;
	}
};

USTRUCT(BlueprintType)
struct FMapInfo
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString MapName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString GridTypeName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MapScale;
};

UENUM(BlueprintType)
enum class EBattleType : uint8
{
	Type, Roll, Both
};

UCLASS()
class INVADED_API ULobbySG : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere)
		FString PlayerName;
	UPROPERTY(VisibleAnywhere)
		FString SaveSlotName;
	UPROPERTY(VisibleAnywhere)
		uint32 UserIndex;
	UPROPERTY(VisibleAnywhere)
		FLinearColor PlayerColor;
	UPROPERTY(VisibleAnywhere)
		FLinearColor FlagColor;
	UPROPERTY(VisibleAnywhere)
		int32 WeaponIndex;
	UPROPERTY(VisibleAnywhere)
		int32 MeshIndex;
	UPROPERTY(VisibleAnywhere)
		int32 FlagTextureIndex;
	UPROPERTY(VisibleAnywhere)
		uint32 NumOfPlayers;
	UPROPERTY(VisibleAnywhere)
		bool bStartWithFlags;
	UPROPERTY(VisibleAnywhere)
		bool bStartWithCards;
	UPROPERTY(VisibleAnywhere)
		bool bStartWithFight;
	UPROPERTY(VisibleAnywhere)
		bool bUseAllFightingTypes;
	UPROPERTY(VisibleAnywhere)
		FMapInfo MapInfo;
	UPROPERTY(VisibleAnywhere)
		EBattleType BattleType;

	ULobbySG();

	UPROPERTY(VisibleAnywhere)
		TMap<int32, FString> PlayerNames;
	UPROPERTY(VisibleAnywhere)
		TMap<int32, FLinearColor> PlayerColors;
	UPROPERTY(VisibleAnywhere)
		TMap<int32, FLinearColor> FlagColors;
	UPROPERTY(VisibleAnywhere)
		TMap<int32, int32> WeaponIndexes;
	UPROPERTY(VisibleAnywhere)
		TMap<int32, int32> MeshIndexes;
	UPROPERTY(VisibleAnywhere)
		TMap<int32, int32> FlagTextureIndexes;
	
	
	
};
