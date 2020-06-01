// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Core/Public/HAL/ThreadSafeCounter.h"
#include "Runtime/Core/Public/HAL/Runnable.h"
#include "GridGeneratorAlg.generated.h"
/**
 * 
 */

UENUM()
enum class ECellType : uint8
{
	CT_None UMETA(DisplayName = "None"),
	CT_PT UMETA(DisplayName ="PT"),
	CT_StartingPoint UMETA(DisplayName = "StartingPoint"),
	CT_Base 	UMETA(DisplayName = "Base"),
	CT_Corner   UMETA(DisplayName = "Corner"),
	CT_Attack 	UMETA(DisplayName = "Attack"),
	CT_Defense	UMETA(DisplayName = "Defense")
};
USTRUCT(BlueprintType)
struct FVectorArray
{
	GENERATED_BODY()
		UPROPERTY(EditDefaultsOnly)
		TArray<FVector2D> Array;
	FVectorArray()
	{

	}
};
USTRUCT(BlueprintType)
struct FGridGeneratorInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	uint8 NumOfColumns = 14;
	UPROPERTY(EditDefaultsOnly)
	uint8 NumOfRows = 14;
	UPROPERTY(EditDefaultsOnly)
	uint8 NumOfAttackCells = 1;
	UPROPERTY(EditDefaultsOnly)
	uint8 NumOfDefenseCells = 1;
	UPROPERTY(BlueprintReadWrite)
	FVector CellSize;
	UPROPERTY(BlueprintReadWrite)
	TArray<FVector2D> Locations;
	UPROPERTY(BlueprintReadWrite)
	TArray<ECellType> CellTypes;
	UPROPERTY(BlueprintReadWrite)
	FVector Start;
	UPROPERTY(BlueprintReadWrite)
	FRotator StartRotation;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float Interval;
	UPROPERTY(EditDefaultsOnly)
	uint8 CheckRange=2; //for selecting attack/defense cells
	UPROPERTY(EditDefaultsOnly)
	TArray< FVectorArray > Quadrants;	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FString GridTypeName;
};

class INVADED_API FGridGeneratorAlg : public FRunnable
{
public:
	static FGridGeneratorAlg* Runnable;

	static void GenerateAttackCells(FGridGeneratorInfo* Data);

	static void GenerateDefenseCells(FGridGeneratorInfo* Data);

	static bool ClearNeighbourhood(FGridGeneratorInfo* Data,uint8 Index, uint8 Range, ECellType CellType);

	FRunnableThread* Thread;

	FThreadSafeCounter StopTaskCounter;
	
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;

	void EnsureCompletion();
	
	static void Shutdown();

	static bool IsThreadFinished();
	static FGridGeneratorAlg* JoyInit(FGridGeneratorInfo* GeneratorConfig);
	FGridGeneratorAlg(FGridGeneratorInfo* GeneratorConfig);
	~FGridGeneratorAlg();
private:
	bool IsFinished() const
	{
		return bFinish;
	}

	void Generate();
	void GenerateBaseGrid();
	void GenerateOuterGrid();
	void SelectCornerCells();
	void GenerateMainHallways();
	void GenerateHallways();
	void SelectAttackCells();
	void SelectDefenseCells();
	void SelectQuadrants();
	static TArray<uint8> GetNeighbours(FGridGeneratorInfo* Data,uint8 Index,bool bOnlyBlank);
	bool bFinish = false;
	FGridGeneratorInfo* GeneratorInfo;
	uint8 in, out; //Starting point from hallway in right and left direction
};
