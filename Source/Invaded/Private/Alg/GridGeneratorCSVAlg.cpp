// Fill out your copyright notice in the Description page of Project Settings.


#include "GridGeneratorCSVAlg.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"
#include "Kismet/GameplayStatics.h"
#include "FileHelper.h"
#include "Paths.h"


FGridGeneratorCSVAlg* FGridGeneratorCSVAlg::Runnable = nullptr;

FGridGeneratorCSVAlg::FGridGeneratorCSVAlg(FGridGeneratorInfo* GeneratorConfig)
{
	GeneratorInfo = GeneratorConfig;
	Thread = FRunnableThread::Create(this, TEXT("FGridGeneratorCSVAlgorithm"), 0, TPri_AboveNormal);
}

FGridGeneratorCSVAlg::~FGridGeneratorCSVAlg()
{
	delete Thread;
	Thread = nullptr;
}

void FGridGeneratorCSVAlg::ReadFromCSV(const FString & FileName)
{
	FString GameDir = FPaths::ProjectDir();

	if (!GameDir.IsEmpty())
	{
		FString CompleteFilePath = GameDir + "Content/" + "GridMaps/" + FileName + ".csv";

		FString FileData = "";
		FFileHelper::LoadFileToString(FileData, *CompleteFilePath);

		if (!FileData.IsEmpty())
		{
			FileData.ParseIntoArray(CSVData, TEXT(";"), 1);
		}
	}
	
}
void FGridGeneratorCSVAlg::ReadConfig(const FString& FileName)
{
	FString GameDir = FPaths::ProjectDir();

	if (!GameDir.IsEmpty())
	{
		FString CompleteFilePath = GameDir + "Content/" + "GridMaps/" + FileName + ".txt";

		FString FileData = "";
		FFileHelper::LoadFileToString(FileData, *CompleteFilePath);

		TArray<FString> ConfigData;

		FileData.ParseIntoArray(ConfigData, TEXT(";"), 1);


		if (ConfigData.Num()>1)
		{


			GeneratorInfo->NumOfRows = FCString::Atoi(*ConfigData[0]);
			GeneratorInfo->NumOfColumns = FCString::Atoi(*ConfigData[1]);
		}
		else
		{
			GeneratorInfo->NumOfRows = 12;
			GeneratorInfo->NumOfColumns = 12;
		}
	}

	
}
void FGridGeneratorCSVAlg::Generate()
{
	if (GeneratorInfo)
	{
		ReadConfig(GeneratorInfo->GridTypeName);

		ReadFromCSV(GeneratorInfo->GridTypeName);

		UE_LOG(LogTemp, Warning, TEXT("Size %d %s"), CSVData.Num(), *GeneratorInfo->GridTypeName);

		if (CSVData.Num())
		{
			GenerateBaseGrid();
			SelectAttackCells();
			SelectDefenseCells();
			SelectQuadrants();

			bFinish = true;
		}
		else
		{
			bFinish = true;
		}
	}

}
void FGridGeneratorCSVAlg::GenerateBaseGrid()
{
	TArray<int32> CornerCellIndexes;
	FVector2D Location = FVector2D(GeneratorInfo->Start);
	for (uint8 j = 0; j < GeneratorInfo->NumOfRows; ++j)
	{
		for (uint8 i = 0; i < GeneratorInfo->NumOfColumns; ++i)
		{
			
				GeneratorInfo->Locations.Add(Location);

				switch (FCString::Atoi(*CSVData[i + j * 12]))
				{
				case 0:
				{
					GeneratorInfo->CellTypes.Add(ECellType::CT_None);
					break;
				}
				case 1:
				{
					GeneratorInfo->CellTypes.Add(ECellType::CT_Base);
					break;
				}
				case 2:
				{
					GeneratorInfo->CellTypes.Add(ECellType::CT_StartingPoint);
					break;
				}
				case 3:
				{
					GeneratorInfo->CellTypes.Add(ECellType::CT_PT);
					CornerCellIndexes.Add(i + j * 12);
					break;
				}

				}
			
			
	
			Location += FVector2D(GeneratorInfo->CellSize.X * 2 + GeneratorInfo->Interval, 0.0f);
		}
		Location = FVector2D(GeneratorInfo->Start.X, Location.Y + GeneratorInfo->CellSize.Y * 2 + GeneratorInfo->Interval);
	}

	TArray<FVector2D> LocationsVisited;
	for (int32 i = 0; i < CornerCellIndexes.Num();++i)
	{
		if (LocationsVisited.Find(GeneratorInfo->Locations[CornerCellIndexes[i]]) == INDEX_NONE)
		{
			FVector2D NewLocation = FVector2D(0.0f, 0.0f);

			uint8 index1 = CornerCellIndexes[i];
			uint8 index2 = index1 + 1;
			uint8 index3 = index1 + GeneratorInfo->NumOfColumns;
			uint8 index4 = index3 + 1;

			LocationsVisited.Add(GeneratorInfo->Locations[index1]);
			LocationsVisited.Add(GeneratorInfo->Locations[index2]);
			LocationsVisited.Add(GeneratorInfo->Locations[index3]);
			LocationsVisited.Add(GeneratorInfo->Locations[index4]);

			NewLocation += GeneratorInfo->Locations[index1];
			NewLocation += GeneratorInfo->Locations[index2];
			NewLocation += GeneratorInfo->Locations[index3];
			NewLocation += GeneratorInfo->Locations[index4];

			NewLocation /= 4;

			GeneratorInfo->Locations.Add(NewLocation);
			GeneratorInfo->CellTypes.Add(ECellType::CT_Corner);
		}		
	}
}
void FGridGeneratorCSVAlg::SelectAttackCells()
{
	FGridGeneratorAlg::GenerateAttackCells(GeneratorInfo);
}

void FGridGeneratorCSVAlg::SelectDefenseCells()
{
	FGridGeneratorAlg::GenerateDefenseCells(GeneratorInfo);
}

void FGridGeneratorCSVAlg::SelectQuadrants()
{
	TArray<int32> StartingCellIndexes;
	for (int32 i = 0; i < GeneratorInfo->CellTypes.Num(); ++i)
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
		for (int32 j = GeneratorInfo->NumOfColumns / 2; j < GeneratorInfo->NumOfColumns; ++j)
		{
			GeneratorInfo->Quadrants[2].Array.Add(FVector2D(GeneratorInfo->Locations[i + j * GeneratorInfo->NumOfRows]));
		}
	}
	for (int32 i = GeneratorInfo->NumOfRows / 2; i < GeneratorInfo->NumOfRows; ++i)//Bottom Right
	{
		for (int32 j = GeneratorInfo->NumOfColumns / 2; j < GeneratorInfo->NumOfColumns; ++j)
		{
			GeneratorInfo->Quadrants[3].Array.Add(FVector2D(GeneratorInfo->Locations[i + j * GeneratorInfo->NumOfRows]));
		}
	}
}

bool FGridGeneratorCSVAlg::Init()
{
	double Secs = FTimespan(FDateTime::Now().GetTicks()).GetTotalSeconds();
	int32 Seed = (int32)(((int64)Secs) % INT_MAX);
	FMath::RandInit(Seed);
	return true;
}

uint32 FGridGeneratorCSVAlg::Run()
{
	FPlatformProcess::Sleep(0.02);
	Generate();
	return 0;
}

void FGridGeneratorCSVAlg::Stop()
{
	StopTaskCounter.Increment();
}

void FGridGeneratorCSVAlg::EnsureCompletion()
{
	Stop();
	Thread->WaitForCompletion();
}

void FGridGeneratorCSVAlg::Shutdown()
{
	if (Runnable)
	{
		Runnable->EnsureCompletion();
		delete Runnable;
		Runnable = nullptr;
	}
}

bool FGridGeneratorCSVAlg::IsThreadFinished()
{
	if (Runnable) return Runnable->IsFinished();
	return false;
}

FGridGeneratorCSVAlg * FGridGeneratorCSVAlg::JoyInit(FGridGeneratorInfo * GeneratorConfig)
{
	if (!Runnable && FPlatformProcess::SupportsMultithreading())
	{
		Runnable = new FGridGeneratorCSVAlg(GeneratorConfig);
	}
	return Runnable;
}
