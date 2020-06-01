// Fill out your copyright notice in the Description page of Project Settings.

#include "RegenerateGrid.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"
#include "Kismet/GameplayStatics.h"
#include "Actors/CellBase.h"
#include "InvadedCharacter.h"
#include "Environment/GridGenerator.h"
#include "Actors/CellCorner.h"

FRegenerateGrid* FRegenerateGrid::Runnable = nullptr;

FRegenerateGrid::FRegenerateGrid(FGridGeneratorInfo* GeneratorConfig, FGridRegeneratorInfo* RegeneratorConfig)
{
	GeneratorInfo = GeneratorConfig;
	RegeneratorInfo = RegeneratorConfig;

	Thread = FRunnableThread::Create(this, TEXT("FRegenerateGrid"), 0, TPri_AboveNormal);
}

FRegenerateGrid::~FRegenerateGrid()
{
	delete Thread;
	Thread = nullptr;
}

void FRegenerateGrid::DisableQuadrant()
{
	
	FVector2D StartingLocation = FVector2D(RegeneratorInfo->StartingLocation);
	int32 Index = RegeneratorInfo->QuadrantToDisable;

	if (GeneratorInfo->Quadrants.Num() > Index&&Index > -1)
	{

		for (FVector2D Location : GeneratorInfo->Quadrants[Index].Array)
		{
			RegeneratorInfo->GridGenerator->CellDisabledLocations.Add(FVector(Location, RegeneratorInfo->GridGenerator->GetActorLocation().Z));
			for (ACellBase* Cell : RegeneratorInfo->CellArray)
			{
				if (Cell)
				{
					//if (Cell->GetCellType() != ECellType::CT_None)
					//{
					//ErrorMargin to do
					if ((FVector2D(Cell->GetActorLocation().X, Cell->GetActorLocation().Y) - Location).Size() < 50)
					{

						//Cell->SetIsWalkable(false);
						//RegeneratorInfo->Causer->ChangeCellHighlight(Cell, false, EHighlightState::HC_Inactive);
						RegeneratorInfo->CellDisabledArray.Add(Cell);
						
						break;
					}
					//}

				}

			}
			for (ACellCorner* CellCorner : RegeneratorInfo->CellCornerArray)
			{
				if (CellCorner)
				{
					for (ACellBase* Cell : CellCorner->GetHiddenCells())
					{
						if (FVector2D(Cell->GetActorLocation().X, Cell->GetActorLocation().Y) == Location)
						{

						//	CellCorner->SetIsWalkable(false);
						//	RegeneratorInfo->Causer->ChangeCellHighlight(CellCorner, false, EHighlightState::HC_Inactive);
							RegeneratorInfo->CellDisabledArray.Add(CellCorner);
							RegeneratorInfo->CellDisabledArray.Append(CellCorner->GetHiddenCells());
							
							break;
						}

					}
				}
			}
		}
	}

	//StartingCell->SetIsWalkable(false);
	//RegeneratorInfo->Causer->ChangeCellHighlight(StartingCell, true, EHighlightState::HC_Inactive);
}

void FRegenerateGrid::GenerateAttackCells(FGridGeneratorInfo * Data)
{
}

void FRegenerateGrid::GenerateDefenseCells(FGridGeneratorInfo * Data)
{
}

bool FRegenerateGrid::ClearNeighbourhood(FGridGeneratorInfo * Data, uint8 Index, uint8 Range, ECellType CellType)
{
	return false;
}

bool FRegenerateGrid::Init()
{
	double Secs = FTimespan(FDateTime::Now().GetTicks()).GetTotalSeconds();
	int32 Seed = (int32)(((int64)Secs) % INT_MAX);
	FMath::RandInit(Seed);
	return true;
}

uint32 FRegenerateGrid::Run()
{
	FPlatformProcess::Sleep(0.02);
	
	DisableQuadrant();

	bFinish = true;
	return 0;
}

void FRegenerateGrid::Stop()
{
	StopTaskCounter.Increment();
}

void FRegenerateGrid::EnsureCompletion()
{
	Stop();
	Thread->WaitForCompletion();
}

void FRegenerateGrid::Shutdown()
{
	if (Runnable)
	{
		Runnable->EnsureCompletion();
		delete Runnable;
		Runnable = nullptr;
	}
}

bool FRegenerateGrid::IsThreadFinished()
{
	if (Runnable) return Runnable->IsFinished();
	return false;
}

FRegenerateGrid * FRegenerateGrid::JoyInit(FGridGeneratorInfo * GeneratorConfig, FGridRegeneratorInfo* RegeneratorConfig)
{
	if (!Runnable && FPlatformProcess::SupportsMultithreading())
	{
		Runnable = new FRegenerateGrid(GeneratorConfig,RegeneratorConfig);
	}
	return Runnable;
}
