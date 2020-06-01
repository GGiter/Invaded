// Fill out your copyright notice in the Description page of Project Settings.


#include "GridGeneratorFunctionLibrary.h"
#include "Engine/World.h"
#include "CellBase.h"
#include "CellAttack.h"
#include "CellDefense.h"
#include "CellCorner.h"
#include "Async.h"
#include "Kismet/GameplayStatics.h"
#include "FileHelper.h"
#include "Paths.h"
#include "HAL/FileManager.h"
#include "JsonSerializerMacros.h"


TArray<FString> AGridGeneratorFunctionLibrary::FindMaps()
{
	FString RelativePath = FPaths::GameContentDir();

	FString FullPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*RelativePath);

	FullPath += "GridMaps";
	FJsonSerializableArray Files;

	IFileManager::Get().FindFiles(Files, *FullPath);

	TArray<FString> Result;
	for (FString File : Files)
	{
		if (File.Find(".txt") != -1)
		{
			File.RemoveFromEnd(".txt");
			Result.Add(File);
		}
		
	}
	return Result;
}
void AGridGeneratorFunctionLibrary::GenerateBuildData(FGridGeneratorInfo GridInfo,float Scale)
{	
	GridConfigRef = &GridInfo;
	FGridGeneratorInfo* GeneratorInfo = GridConfigRef;
	
	FString FileName = GridInfo.GridTypeName;
	this->Scale = Scale;
	
	GridInfo.Locations.Empty();
	GridInfo.CellTypes.Empty();

	FActorSpawnParameters SpawnParams;
	FVector Origin;
	FTransform Transform(GetActorRotation(), GetActorLocation(), FVector(1.0f, 1.0f, 1.0f)*Scale);
	ACellBase* Cell = GetWorld()->SpawnActor<ACellBase>(CellBaseClass, Transform, SpawnParams);
	Cell->GetActorBounds(false, Origin, GridInfo.CellSize);
	Cell->Destroy();

	//AsyncTask(ENamedThreads::GameThread, [=]() {

		FGridGeneratorInfo GridInfoCopy = GridInfo;
		

		FString GameDir = FPaths::ProjectDir();
		TArray<FString> CSVData;
		if (!GameDir.IsEmpty())
		{
			FString CompleteFilePath = GameDir + "Content/" + "GridMaps/" + FileName + ".txt";

			FString FileData = "";
			FFileHelper::LoadFileToString(FileData, *CompleteFilePath);

			

			TArray<FString> ConfigData;

			FileData.ParseIntoArray(ConfigData, TEXT(";"), 0);

		
			if (ConfigData.Num())
			{
				GridInfoCopy.NumOfRows = FCString::Atoi(*ConfigData[0]);
				GridInfoCopy.NumOfColumns = FCString::Atoi(*ConfigData[1]);
			}
			else
			{
				GridInfoCopy.NumOfRows = 12;
				GridInfoCopy.NumOfColumns = 12;
			}
		}

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
		UE_LOG(LogTemp, Warning, TEXT("Start %d %d"),CSVData.Num(), (GridInfoCopy.NumOfRows - 1) + (GridInfoCopy.NumOfColumns - 1) * GridInfoCopy.NumOfRows);
		if (CSVData.Num() >= (GridInfoCopy.NumOfRows - 1)+(GridInfoCopy.NumOfColumns - 1) * GridInfoCopy.NumOfRows)
		{
			TArray<int32> CornerCellIndexes;
			FVector2D Location = FVector2D(GridInfoCopy.Start);
			for (uint8 j = 0; j < GridInfoCopy.NumOfRows; ++j)
			{
				for (uint8 i = 0; i < GridInfoCopy.NumOfColumns; ++i)
				{

					GridInfoCopy.Locations.Add(Location);

					switch (FCString::Atoi(*CSVData[i + j * GridInfoCopy.NumOfRows]))
					{
					case 0:
					{
						GridInfoCopy.CellTypes.Add(ECellType::CT_None);
						break;
					}
					case 1:
					{
						GridInfoCopy.CellTypes.Add(ECellType::CT_Base);
						break;
					}
					case 2:
					{
						GridInfoCopy.CellTypes.Add(ECellType::CT_StartingPoint);
						break;
					}
					case 3:
					{
						GridInfoCopy.CellTypes.Add(ECellType::CT_PT);
						CornerCellIndexes.Add(i + j * GridInfoCopy.NumOfRows);
						break;
					}

					}



					Location += FVector2D(GridInfoCopy.CellSize.X * 2 + GridInfoCopy.Interval, 0.0f);
				}
				Location = FVector2D(GridInfoCopy.Start.X, Location.Y + GridInfoCopy.CellSize.Y * 2 + GridInfoCopy.Interval);
			}

			TArray<FVector2D> LocationsVisited;
			for (int32 i = 0; i < CornerCellIndexes.Num(); ++i)
			{
				if (LocationsVisited.Find(GridInfoCopy.Locations[CornerCellIndexes[i]]) == INDEX_NONE)
				{
					FVector2D NewLocation = FVector2D(0.0f, 0.0f);

					uint8 index1 = CornerCellIndexes[i];
					uint8 index2 = index1 + 1;
					uint8 index3 = index1 + GridInfoCopy.NumOfColumns;
					uint8 index4 = index3 + 1;

					LocationsVisited.Add(GridInfoCopy.Locations[index1]);
					LocationsVisited.Add(GridInfoCopy.Locations[index2]);
					LocationsVisited.Add(GridInfoCopy.Locations[index3]);
					LocationsVisited.Add(GridInfoCopy.Locations[index4]);

					NewLocation += GridInfoCopy.Locations[index1];
					NewLocation += GridInfoCopy.Locations[index2];
					NewLocation += GridInfoCopy.Locations[index3];
					NewLocation += GridInfoCopy.Locations[index4];

					NewLocation /= 4;

					GridInfoCopy.Locations.Add(NewLocation);
					GridInfoCopy.CellTypes.Add(ECellType::CT_Corner);
				}
			}

			TArray<int32> StartingCellIndexes;
			for (int32 i = 0; i < GridInfoCopy.CellTypes.Num(); ++i)
			{
				if (GridInfoCopy.CellTypes[i] == ECellType::CT_StartingPoint)
				{
					StartingCellIndexes.Add(i);
					GridInfoCopy.Quadrants.Add(FVectorArray());
				}

			}

			for (int32 i = 0; i < GridInfoCopy.NumOfRows / 2; ++i)//Upper Left
			{
				for (int32 j = 0; j < GridInfoCopy.NumOfColumns / 2; ++j)
				{
					GridInfoCopy.Quadrants[0].Array.Add(FVector2D(GridInfoCopy.Locations[i + j * GridInfoCopy.NumOfRows]));
				}
			}
			for (int32 i = GridInfoCopy.NumOfRows / 2; i < GridInfoCopy.NumOfRows; ++i)//Upper Right
			{
				for (int32 j = 0; j < GridInfoCopy.NumOfColumns / 2; ++j)
				{
					GridInfoCopy.Quadrants[1].Array.Add(FVector2D(GridInfoCopy.Locations[i + j * GridInfoCopy.NumOfRows]));
				}
			}
			for (int32 i = 0; i < GridInfoCopy.NumOfRows / 2; ++i)//Botton Left
			{
				for (int32 j = GridInfoCopy.NumOfColumns / 2; j < GridInfoCopy.NumOfColumns; ++j)
				{
					GridInfoCopy.Quadrants[2].Array.Add(FVector2D(GridInfoCopy.Locations[i + j * GridInfoCopy.NumOfRows]));
				}
			}
			for (int32 i = GridInfoCopy.NumOfRows / 2; i < GridInfoCopy.NumOfRows; ++i)//Bottom Right
			{
				for (int32 j = GridInfoCopy.NumOfColumns / 2; j < GridInfoCopy.NumOfColumns; ++j)
				{
					GridInfoCopy.Quadrants[3].Array.Add(FVector2D(GridInfoCopy.Locations[i + j * GridInfoCopy.NumOfRows]));
				}
			}

			TArray<FTransform> Transforms;

			for (uint8 i = 0; i < GridInfoCopy.Locations.Num(); ++i)
			{
				FTransform Transform(GridInfoCopy.StartRotation, FVector(GridInfoCopy.Locations[i].X, GridInfoCopy.Locations[i].Y, GridInfoCopy.Start.Z), FVector(1.0f, 1.0f, 1.0f)*Scale);
				Transforms.Add(Transform);
			}
			int32 i = 0;
			for (FTransform Transform : Transforms)
			{

				FActorSpawnParameters SpawnParams;
				if (GridInfoCopy.CellTypes[i] == ECellType::CT_Base)
				{
					ACellBase* Cell = GetWorld()->SpawnActor<ACellBase>(CellBaseClass, Transform, SpawnParams);
					Cell->SetReplicates(true);
					CellArray.Add(Cell);
				}
				else if (GridInfoCopy.CellTypes[i] == ECellType::CT_Attack)
				{
					ACellAttack* Cell = GetWorld()->SpawnActor<ACellAttack>(CellAttackClass, Transform, SpawnParams);
					Cell->SetReplicates(true);
					CellArray.Add(Cell);
				}
				else if (GridInfoCopy.CellTypes[i] == ECellType::CT_Defense)
				{
					ACellDefense* Cell = GetWorld()->SpawnActor<ACellDefense>(CellDefenseClass, Transform, SpawnParams);
					Cell->SetReplicates(true);
					CellArray.Add(Cell);
				}
				else if (GridInfoCopy.CellTypes[i] == ECellType::CT_Corner)
				{
					ACellCorner* Cell = GetWorld()->SpawnActor<ACellCorner>(CellCornerClass, Transform, SpawnParams);
					Cell->SetReplicates(true);
					CellArray.Add(Cell);
				}
				else if (GridInfoCopy.CellTypes[i] == ECellType::CT_StartingPoint)
				{
					ACellBase* Cell = GetWorld()->SpawnActor<ACellBase>(CellBaseClass, Transform, SpawnParams);
					Cell->SetReplicates(true);
					CellArray.Add(Cell);
				}
				i++;
		}
		
		}

		UE_LOG(LogTemp, Warning, TEXT("Finish %d"), CellArray.Num());
	//});
	
		
	
	
}

void AGridGeneratorFunctionLibrary::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	FGridGeneratorAlg::Shutdown();
	FGridGeneratorCSVAlg::Shutdown();

	Super::EndPlay(EndPlayReason);
}

FGridBuildData AGridGeneratorFunctionLibrary::GetBuildData()
{	
	FGridGeneratorInfo GridConfig = *GridConfigRef;

	TArray<FVector2D> Locations2D = GridConfig.Locations;
	TArray<ECellType> CellTypes = GridConfig.CellTypes;
	TArray<FTransform> Transforms;

	for (uint8 i = 0; i < Locations2D.Num(); ++i)
	{
		FTransform Transform(GridConfig.StartRotation, FVector(Locations2D[i].X, Locations2D[i].Y, GridConfig.Start.Z), FVector(1.0f, 1.0f, 1.0f)*Scale);
		Transforms.Add(Transform);
	}

	return FGridBuildData(Transforms, CellTypes);
}

void AGridGeneratorFunctionLibrary::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);

	if (FGridGeneratorAlg::IsThreadFinished() || FGridGeneratorCSVAlg::IsThreadFinished())
	{
		FGridGeneratorAlg::Shutdown();
		FGridGeneratorCSVAlg::Shutdown();

		BuildData(GetBuildData());
	}
}

void AGridGeneratorFunctionLibrary::BuildData(FGridBuildData BuildData)
{
	if (CellBaseClass.Get() && CellAttackClass.Get() && CellDefenseClass.Get() && CellCornerClass.Get())
	{
		UE_LOG(LogTemp, Warning, TEXT("BuildData %d"), BuildData.Transforms.Num());
		int32 i = 0;
		for (FTransform Transform : BuildData.Transforms)
		{

			FActorSpawnParameters SpawnParams;
			if (BuildData.CellTypes[i] == ECellType::CT_Base)
			{
				ACellBase* Cell = GetWorld()->SpawnActor<ACellBase>(CellBaseClass, Transform, SpawnParams);
				CellArray.Add(Cell);
			}
			else if (BuildData.CellTypes[i] == ECellType::CT_Attack)
			{
				ACellAttack* Cell = GetWorld()->SpawnActor<ACellAttack>(CellAttackClass, Transform, SpawnParams);
				CellArray.Add(Cell);
			}
			else if (BuildData.CellTypes[i] == ECellType::CT_Defense)
			{
				ACellDefense* Cell = GetWorld()->SpawnActor<ACellDefense>(CellDefenseClass, Transform, SpawnParams);
				CellArray.Add(Cell);
			}
			else if (BuildData.CellTypes[i] == ECellType::CT_Corner)
			{
				ACellCorner* Cell = GetWorld()->SpawnActor<ACellCorner>(CellCornerClass, Transform, SpawnParams);
				CellArray.Add(Cell);
			}
			else if (BuildData.CellTypes[i] == ECellType::CT_StartingPoint)
			{
				ACellBase* Cell = GetWorld()->SpawnActor<ACellBase>(CellBaseClass, Transform, SpawnParams);
				CellArray.Add(Cell);
			}
			i++;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("One or more of Cell classes aren't set"));
	}
	
}
void AGridGeneratorFunctionLibrary::DestroyCells()
{
	for (ACellBase* Cell : CellArray)
	{
		if (Cell)
		{
			Cell->Destroy();
		}
		
	}
	CellArray.Empty();
}

