// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridGeneratorAlg.h"
#include "Runtime/Core/Public/HAL/ThreadSafeCounter.h"
#include "Runtime/Core/Public/HAL/Runnable.h"


/**
 * 
 */

class INVADED_API FGridGeneratorCSVAlg : public FRunnable
{
public:
	static FGridGeneratorCSVAlg* Runnable;

	FRunnableThread* Thread;

	FThreadSafeCounter StopTaskCounter;

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;

	void EnsureCompletion();

	static void Shutdown();

	static bool IsThreadFinished();
	static FGridGeneratorCSVAlg* JoyInit(FGridGeneratorInfo* GeneratorConfig);

	FGridGeneratorCSVAlg(FGridGeneratorInfo* GeneratorConfig);

	~FGridGeneratorCSVAlg();

private:
	bool IsFinished() const
	{
		return bFinish;
	}

	void ReadFromCSV(const FString& FileName);
	void ReadConfig(const FString& FileName);
	void Generate();
	void GenerateBaseGrid();
	void SelectAttackCells();
	void SelectDefenseCells();
	void SelectQuadrants();

	bool bFinish = false;
	FGridGeneratorInfo* GeneratorInfo;

	TArray<FString> CSVData;

};
