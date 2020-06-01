// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "Runtime/Core/Public/HAL/ThreadSafeCounter.h"
#include "Runtime/Core/Public/HAL/Runnable.h"
#include "GridGeneratorAlg.h"
#include "RegenerateGrid.generated.h"


/**
 * 
 */
USTRUCT()
struct FGridRegeneratorInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
		class AInvadedCharacter* Causer;

	UPROPERTY(EditDefaultsOnly)
		int32 QuadrantToDisable;

	UPROPERTY(EditDefaultsOnly)
		class AGridGenerator* GridGenerator;

	UPROPERTY(EditDefaultsOnly)
		FVector StartingLocation;

	UPROPERTY(EditDefaultsOnly)
		TArray<class ACellBase*> CellArray;

	UPROPERTY(EditDefaultsOnly)
		TArray<class ACellCorner*> CellCornerArray;

	UPROPERTY(EditDefaultsOnly)
		TArray<class ACellBase*> CellDisabledArray;

};


class INVADED_API FRegenerateGrid : public FRunnable
{
public:
	static FRegenerateGrid* Runnable;

	FRunnableThread* Thread;

	FThreadSafeCounter StopTaskCounter;

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;

	void EnsureCompletion();

	static void Shutdown();

	static bool IsThreadFinished();
	static FRegenerateGrid* JoyInit(FGridGeneratorInfo* GeneratorConfig, FGridRegeneratorInfo* RegeneratorConfig);
	FRegenerateGrid(FGridGeneratorInfo* GeneratorConfig, FGridRegeneratorInfo* RegeneratorConfig);
	~FRegenerateGrid();
private:
	void DisableQuadrant();

	void GenerateAttackCells(FGridGeneratorInfo* Data);

	void GenerateDefenseCells(FGridGeneratorInfo* Data);

	bool ClearNeighbourhood(FGridGeneratorInfo* Data, uint8 Index, uint8 Range, ECellType CellType);

	bool IsFinished() const
	{
		return bFinish;
	}
	bool bFinish = false;

	FGridGeneratorInfo* GeneratorInfo;
	FGridRegeneratorInfo* RegeneratorInfo;
};
