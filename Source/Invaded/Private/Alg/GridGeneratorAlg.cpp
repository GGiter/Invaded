// Fill out your copyright notice in the Description page of Project Settings.

#include "GridGeneratorAlg.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"
#include "Kismet/GameplayStatics.h"

FGridGeneratorAlg* FGridGeneratorAlg::Runnable = nullptr;

void FGridGeneratorAlg::GenerateAttackCells(FGridGeneratorInfo * Data)
{
	uint8 j = 0;
	while (j < Data->NumOfAttackCells)
	{
		int Index = FMath::RoundToInt(FMath::FRandRange(0.0f, Data->Locations.Num() - 1));
		if (Data->CellTypes[Index] == ECellType::CT_Base&&ClearNeighbourhood(Data,Index, Data->CheckRange, ECellType::CT_Attack))
		{
			Data->CellTypes[Index] = ECellType::CT_Attack;
			j++;
		}

	}
	
}

void FGridGeneratorAlg::GenerateDefenseCells(FGridGeneratorInfo * Data)
{
	uint8 j = 0;
	while (j < Data->NumOfDefenseCells)
	{
		int Index = FMath::RoundToInt(FMath::FRandRange(0.0f, Data->Locations.Num() - 1));
		if (Data->CellTypes[Index] == ECellType::CT_Base&&ClearNeighbourhood(Data,Index, Data->CheckRange, ECellType::CT_Defense))
		{
			Data->CellTypes[Index] = ECellType::CT_Defense;
			j++;
		}

	}
	
}

bool FGridGeneratorAlg::Init()
{
	double Secs = FTimespan(FDateTime::Now().GetTicks()).GetTotalSeconds();
	int32 Seed = (int32)(((int64)Secs) % INT_MAX);
	FMath::RandInit(Seed);
	return true;
}

uint32 FGridGeneratorAlg::Run()
{
	FPlatformProcess::Sleep(0.02);
	Generate();
	return 0;
}
void FGridGeneratorAlg::Generate()
{
	GenerateBaseGrid();
	GenerateOuterGrid();
	SelectCornerCells();
	GenerateMainHallways();
	GenerateHallways();
	SelectAttackCells();
	SelectDefenseCells();
	SelectQuadrants();
		
	bFinish = true;
}
void FGridGeneratorAlg::GenerateBaseGrid()
{
	FVector2D Location = FVector2D(GeneratorInfo->Start);
	for (uint8 j = 0; j < GeneratorInfo->NumOfRows; ++j)
	{
		for (uint8 i = 0; i < GeneratorInfo->NumOfColumns; ++i)
		{
			GeneratorInfo->Locations.Add(Location);
			GeneratorInfo->CellTypes.Add(ECellType::CT_None);
			Location += FVector2D(GeneratorInfo->CellSize.X * 2 + GeneratorInfo->Interval, 0.0f);
		}
		Location = FVector2D(GeneratorInfo->Start.X, Location.Y + GeneratorInfo->CellSize.Y * 2 + GeneratorInfo->Interval);
	}
}
void FGridGeneratorAlg::GenerateOuterGrid()
{
	for (uint8 j = 0; j < GeneratorInfo->NumOfRows; j += GeneratorInfo->NumOfRows - 1)
	{
		for (uint8 i = 0; i < GeneratorInfo->NumOfColumns; ++i)
		{
			GeneratorInfo->CellTypes[i + j * GeneratorInfo->NumOfRows] = ECellType::CT_Base;
		}
	}
	for (uint8 j = 0; j < GeneratorInfo->NumOfRows; ++j)
	{
		for (uint8 i = 0; i < GeneratorInfo->NumOfColumns; i += GeneratorInfo->NumOfColumns - 1)
		{
			GeneratorInfo->CellTypes[i + j * GeneratorInfo->NumOfRows] = ECellType::CT_Base;
		}
	}
}
void FGridGeneratorAlg::SelectCornerCells()
{
	for (uint8 j = 0; j < GeneratorInfo->NumOfRows; j += GeneratorInfo->NumOfRows - 2)
	{
		for (uint8 i = 0; i < GeneratorInfo->NumOfColumns; i += GeneratorInfo->NumOfColumns - 2)
		{
			FVector2D Location = FVector2D(0.0f, 0.0f);
			uint8 index1 = i + j * GeneratorInfo->NumOfRows;
			uint8 index2 = i + 1 + j * GeneratorInfo->NumOfRows;
			uint8 index3 = i + (j + 1) * GeneratorInfo->NumOfRows;
			uint8 index4 = i + 1 + (j + 1) * GeneratorInfo->NumOfRows;
			Location += GeneratorInfo->Locations[index1];
			Location += GeneratorInfo->Locations[index2];
			Location += GeneratorInfo->Locations[index3];
			Location += GeneratorInfo->Locations[index4];

			Location /= 4;

			GeneratorInfo->CellTypes[index1] = ECellType::CT_PT;
			GeneratorInfo->CellTypes[index2] = ECellType::CT_PT;
			GeneratorInfo->CellTypes[index3] = ECellType::CT_PT;
			GeneratorInfo->CellTypes[index4] = ECellType::CT_PT;

			GeneratorInfo->Locations.Add(Location);
			GeneratorInfo->CellTypes.Add(ECellType::CT_Corner);
		}
	}
}
void FGridGeneratorAlg::GenerateMainHallways()
{
	if (FMath::Rand() < 0.50f)
	{
		in = 7;
		out = 6;
	}
	else
	{
		in = 6;
		out = 7;
	}
	for (uint8 i = in; i < in + GeneratorInfo->NumOfColumns / 2 * GeneratorInfo->NumOfColumns; i += GeneratorInfo->NumOfColumns)
	{
		GeneratorInfo->CellTypes[i] = ECellType::CT_Base;
		if (i == in + (GeneratorInfo->NumOfColumns / 2 - 1) * GeneratorInfo->NumOfColumns)
			GeneratorInfo->CellTypes[i] = ECellType::CT_StartingPoint;
	}

	for (uint8 i = out + GeneratorInfo->NumOfColumns*(GeneratorInfo->NumOfColumns - 1); i >= out + GeneratorInfo->NumOfColumns*GeneratorInfo->NumOfColumns / 2; i -= GeneratorInfo->NumOfColumns)
	{
		GeneratorInfo->CellTypes[i] = ECellType::CT_Base;
		if (i == out + GeneratorInfo->NumOfColumns*GeneratorInfo->NumOfColumns / 2)
			GeneratorInfo->CellTypes[i] = ECellType::CT_StartingPoint;
	}
	for (uint8 i = in * GeneratorInfo->NumOfColumns + GeneratorInfo->NumOfColumns - 1; i >= in * GeneratorInfo->NumOfColumns + GeneratorInfo->NumOfColumns - GeneratorInfo->NumOfColumns / 2; i -= 1)
	{
		GeneratorInfo->CellTypes[i] = ECellType::CT_Base;
		if (i == in * GeneratorInfo->NumOfColumns + GeneratorInfo->NumOfColumns - GeneratorInfo->NumOfColumns / 2)
			GeneratorInfo->CellTypes[i] = ECellType::CT_StartingPoint;
	}
	for (uint8 i = out * GeneratorInfo->NumOfColumns; i < out * GeneratorInfo->NumOfColumns + GeneratorInfo->NumOfColumns - GeneratorInfo->NumOfColumns / 2; i += 1)
	{
		GeneratorInfo->CellTypes[i] = ECellType::CT_Base;
		if (i == out * GeneratorInfo->NumOfColumns + GeneratorInfo->NumOfColumns - GeneratorInfo->NumOfColumns / 2 - 1)
			GeneratorInfo->CellTypes[i] = ECellType::CT_StartingPoint;
	}
}
void FGridGeneratorAlg::GenerateHallways()
{
	TArray<TArray<uint8 >> StartingCells;
	StartingCells.Add(TArray<uint8>());//BottomLeft
	StartingCells.Add(TArray<uint8>());//BottomRight
	StartingCells.Add(TArray<uint8>());//UpperLeft
	StartingCells.Add(TArray<uint8>());//UpperRight
	for (uint8 j = 3; j < GeneratorInfo->NumOfRows - 3; ++j)
	{

		for (uint8 i = 3; i < GeneratorInfo->NumOfColumns - 3; ++i)
		{

			if (GetNeighbours(GeneratorInfo,j*GeneratorInfo->NumOfColumns + i,true).Num() == 8)
			{
				if (i < GeneratorInfo->NumOfColumns / 2 && j < GeneratorInfo->NumOfRows / 2)
				{
					StartingCells[0].Add(j*GeneratorInfo->NumOfColumns + i);
				}
				else if (i < GeneratorInfo->NumOfColumns / 2 && j > GeneratorInfo->NumOfRows / 2)
				{
					StartingCells[2].Add(j*GeneratorInfo->NumOfColumns + i);
				}
				else if (i > GeneratorInfo->NumOfColumns / 2 && j < GeneratorInfo->NumOfRows / 2)
				{
					StartingCells[1].Add(j*GeneratorInfo->NumOfColumns + i);
				}
				else
				{
					StartingCells[3].Add(j*GeneratorInfo->NumOfColumns + i);
				}

			}




		}
	}

	//Hallways from random points
	for (uint8 i = 0; i < 4; ++i)
	{
		uint8 Index = StartingCells[i][FMath::RoundToInt(FMath::FRandRange(0.0f, StartingCells[i].Num() - 1))];
		GeneratorInfo->CellTypes[Index] = ECellType::CT_Base;
		if (i == 0)
		{
			for (uint8 j = Index / GeneratorInfo->NumOfColumns*GeneratorInfo->NumOfColumns; j < Index / GeneratorInfo->NumOfColumns*GeneratorInfo->NumOfColumns + in; j++)
			{
				GeneratorInfo->CellTypes[j] = ECellType::CT_Base;
			}
			for (uint8 j = Index % GeneratorInfo->NumOfColumns; j < Index % GeneratorInfo->NumOfColumns + out * GeneratorInfo->NumOfColumns; j += GeneratorInfo->NumOfColumns)
			{
				GeneratorInfo->CellTypes[j] = ECellType::CT_Base;
			}
		}
		else if (i == 2)
		{
			for (uint8 j = Index / GeneratorInfo->NumOfColumns*GeneratorInfo->NumOfColumns; j < Index / GeneratorInfo->NumOfColumns*GeneratorInfo->NumOfColumns + out; j++)
			{
				GeneratorInfo->CellTypes[j] = ECellType::CT_Base;
			}
			for (uint8 j = Index % GeneratorInfo->NumOfColumns + out * GeneratorInfo->NumOfColumns; j < Index % GeneratorInfo->NumOfColumns + (GeneratorInfo->NumOfColumns - 1) * GeneratorInfo->NumOfColumns; j += GeneratorInfo->NumOfColumns)
			{
				GeneratorInfo->CellTypes[j] = ECellType::CT_Base;
			}

		}
		else if (i == 1)
		{
			for (uint8 j = Index / GeneratorInfo->NumOfColumns*GeneratorInfo->NumOfColumns + in; j < Index / GeneratorInfo->NumOfColumns*GeneratorInfo->NumOfColumns + GeneratorInfo->NumOfColumns - 1; j++)
			{
				GeneratorInfo->CellTypes[j] = ECellType::CT_Base;
			}
			for (uint8 j = Index % GeneratorInfo->NumOfColumns; j < Index % GeneratorInfo->NumOfColumns + in * GeneratorInfo->NumOfColumns; j += GeneratorInfo->NumOfColumns)
			{
				GeneratorInfo->CellTypes[j] = ECellType::CT_Base;
			}
		}
		else if (i == 3)
		{
			for (uint8 j = Index / GeneratorInfo->NumOfColumns*GeneratorInfo->NumOfColumns + out; j < Index / GeneratorInfo->NumOfColumns*GeneratorInfo->NumOfColumns + GeneratorInfo->NumOfColumns - 1; j++)
			{
				GeneratorInfo->CellTypes[j] = ECellType::CT_Base;
			}
			for (uint8 j = Index % GeneratorInfo->NumOfColumns + out * GeneratorInfo->NumOfColumns; j < Index % GeneratorInfo->NumOfColumns + (GeneratorInfo->NumOfColumns - 1) * GeneratorInfo->NumOfColumns; j += GeneratorInfo->NumOfColumns)
			{
				GeneratorInfo->CellTypes[j] = ECellType::CT_Base;
			}
		}
	}
}
void FGridGeneratorAlg::SelectAttackCells()
{
	GenerateAttackCells(GeneratorInfo);
}
void FGridGeneratorAlg::SelectDefenseCells()
{
	GenerateDefenseCells(GeneratorInfo);
}
void FGridGeneratorAlg::SelectQuadrants()
{
	TArray<int32> StartingCellIndexes;
	for (int32 i=0;i<GeneratorInfo->CellTypes.Num();++i)
	{
		if (GeneratorInfo->CellTypes[i] == ECellType::CT_StartingPoint)
		{
			StartingCellIndexes.Add(i);
			GeneratorInfo->Quadrants.Add(FVectorArray());
		}
			
	}
	for (int32 i = 0; i < GeneratorInfo->NumOfRows / 2; ++i)//Upper Left
	{
		for (int32 j = 0; j < GeneratorInfo->NumOfColumns / 2; ++j)
		{
			GeneratorInfo->Quadrants[0].Array.Add(FVector2D(GeneratorInfo->Locations[i + j * GeneratorInfo->NumOfRows]));
		}
	}
	for (int32 i = GeneratorInfo->NumOfRows / 2; i < GeneratorInfo->NumOfRows; ++i)//Upper Right
	{
		for (int32 j = 0; j < GeneratorInfo->NumOfColumns / 2; ++j)
		{
			GeneratorInfo->Quadrants[1].Array.Add(FVector2D(GeneratorInfo->Locations[i + j * GeneratorInfo->NumOfRows]));
		}
	}
	for (int32 i = 0; i < GeneratorInfo->NumOfRows / 2; ++i)//Botton Left
	{
		for (int32 j = GeneratorInfo->NumOfColumns / 2; j < GeneratorInfo->NumOfColumns ; ++j)
		{
			GeneratorInfo->Quadrants[2].Array.Add(FVector2D(GeneratorInfo->Locations[i + j * GeneratorInfo->NumOfRows]));
		}
	}
	for (int32 i = GeneratorInfo->NumOfRows / 2; i < GeneratorInfo->NumOfRows ; ++i)//Bottom Right
	{
		for (int32 j = GeneratorInfo->NumOfColumns / 2; j < GeneratorInfo->NumOfColumns; ++j)
		{
			GeneratorInfo->Quadrants[3].Array.Add(FVector2D(GeneratorInfo->Locations[i + j * GeneratorInfo->NumOfRows]));
		}
	}

}
void FGridGeneratorAlg::Stop()
{
	StopTaskCounter.Increment();
}

void FGridGeneratorAlg::EnsureCompletion()
{
	Stop();
	Thread->WaitForCompletion();
}

void FGridGeneratorAlg::Shutdown()
{
	if (Runnable)
	{
		Runnable->EnsureCompletion();
		delete Runnable;
		Runnable = nullptr;
	}
}

bool FGridGeneratorAlg::IsThreadFinished()
{
	if (Runnable) return Runnable->IsFinished();
	return false;
}

FGridGeneratorAlg * FGridGeneratorAlg::JoyInit(FGridGeneratorInfo* GeneratorConfig)
{
	if (!Runnable && FPlatformProcess::SupportsMultithreading())
	{
		Runnable = new FGridGeneratorAlg(GeneratorConfig);
	}
	return Runnable;
}

FGridGeneratorAlg::FGridGeneratorAlg(FGridGeneratorInfo* GeneratorConfig)
{
	GeneratorInfo = GeneratorConfig;
	Thread = FRunnableThread::Create(this, TEXT("FGridGeneratorAlg"), 0, TPri_AboveNormal);
}

FGridGeneratorAlg::~FGridGeneratorAlg()
{
	delete Thread;
	Thread = nullptr;
}
bool FGridGeneratorAlg::ClearNeighbourhood(FGridGeneratorInfo* Data,uint8 Index,uint8 Range,ECellType CellType)
{
	for (uint8 i : GetNeighbours(Data,Index,false))
	{
		if (Data->CellTypes[i] == ECellType::CT_Defense || Data->CellTypes[i] == ECellType::CT_Attack)
		{
			return false;
		}
		if (Range > 1)
		{
			bool ReturnValue = ClearNeighbourhood(Data,i, Range - 1,CellType);
			if (!ReturnValue)
				return false;
		}
		
	}
	return true;
}
TArray<uint8> FGridGeneratorAlg::GetNeighbours(FGridGeneratorInfo* Data,uint8 Index,bool bOnlyBlank)
{
	TArray<uint8> Neighbours;
	for (int8 x = -1; x <= 1; ++x)
	{
		for (int8 y = -1; y <= 1; ++y)
		{
			if ((x == 0 && y == 0))
				continue;
			if (Index + y + x * Data->NumOfColumns > 0 && Index + y + x * Data->NumOfColumns < Data->Locations.Num())
			{
				if (bOnlyBlank)
				{
					if (Data->CellTypes[Index + y + x * Data->NumOfColumns] == ECellType::CT_None)
					{
						Neighbours.Add(Index + y + x * Data->NumOfColumns);
					}
				}
				else
				{
					Neighbours.Add(Index + y + x * Data->NumOfColumns);
				}
				
			}
			
		}
	}
	return Neighbours;
}
