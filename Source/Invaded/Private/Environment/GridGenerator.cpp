// Fill out your copyright notice in the Description page of Project Settings.

#include "GridGenerator.h"
#include "CellBase.h"
#include "CellAttack.h"
#include "CellDefense.h"
#include "CellCorner.h"
#include "UnrealNetwork.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "InvadedPlayer.h"
#include "InvadedGM.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Flag.h"
#include "InvadedPlayerStart.h"
#include "InvadedGS.h"
#include "Player/InvadedMovementComp.h"
#include "AI/InvadedAIPlayer.h"
#include "Alg/RegenerateGrid.h"
#include "Alg/GridGeneratorCSVAlg.h"

float AGridGenerator::MapScale = 1.0f;
// Sets default values
AGridGenerator::AGridGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GridStart = CreateDefaultSubobject<USceneComponent>(TEXT("GridStart"));
	RootComponent = GridStart;
	GridConfig.Interval = 20.0f;
	bReplicates = true;
}
void AGridGenerator::GenerationFinished_Implementation()
{

	
	AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (CellBaseClass)
	{
		TArray<FVector2D> Locations = GridConfig.Locations;
		TArray<ECellType> CellTypes = GridConfig.CellTypes;
		TArray<ACellBase*> HiddenCells;
		for (uint8 i = 0; i < Locations.Num(); ++i)
		{
			FTransform Transform(GetActorRotation(), FVector(Locations[i].X, Locations[i].Y, GetActorLocation().Z), FVector(1.0f, 1.0f, 1.0f)*MapScale);
			FActorSpawnParameters SpawnParams;
			SpawnParams.bNoFail = true;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			if (CellTypes[i] == ECellType::CT_Base)
			{

				ACellBase* Cell = GetWorld()->SpawnActor<ACellBase>(CellBaseClass, Transform, SpawnParams);
				Cell->SetCellType(ECellType::CT_Base);
				CellBaseArray.Add(Cell);
				CellArray.Add(Cell);
				Cell->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

				CellBaseLocation.Add(FVector2D(Locations[i].X, Locations[i].Y));
			}
			else if (CellTypes[i] == ECellType::CT_Attack)
			{
				ACellAttack* Cell = GetWorld()->SpawnActor<ACellAttack>(CellAttackClass, Transform, SpawnParams);
				CellAttackArray.Add(Cell);
				Cell->SetCellType(ECellType::CT_Attack);
				CellArray.Add(Cell);
				Cell->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

				CellBaseLocation.Add(FVector2D(Locations[i].X, Locations[i].Y));
			}
			else if (CellTypes[i] == ECellType::CT_Defense)
			{
				ACellDefense* Cell = GetWorld()->SpawnActor<ACellDefense>(CellDefenseClass, Transform, SpawnParams);
				CellDefenseArray.Add(Cell);
				Cell->SetCellType(ECellType::CT_Defense);
				CellArray.Add(Cell);
				Cell->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

				CellBaseLocation.Add(FVector2D(Locations[i].X, Locations[i].Y));
			}
			else if (CellTypes[i] == ECellType::CT_Corner)
			{

				ACellCorner* Cell = GetWorld()->SpawnActor<ACellCorner>(CellCornerClass, Transform, SpawnParams);;
				Cell->SetCellType(ECellType::CT_Corner);
				CellArray.Add(Cell);
				CellCornerArray.Add(Cell);
				Cell->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
				CellBaseLocation.Add(FVector2D(Locations[i].X, Locations[i].Y));
			}
			else if (CellTypes[i] == ECellType::CT_StartingPoint)
			{
				ACellBase* Cell = GetWorld()->SpawnActor<ACellBase>(CellBaseClass, Transform, SpawnParams);
				Cell->SetCellType(ECellType::CT_StartingPoint);
				StartingCellArray.Add(Cell);
				CellArray.Add(Cell);
				Cell->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
				CellBaseLocation.Add(FVector2D(Locations[i].X, Locations[i].Y));

			}
			else if (CellTypes[i] == ECellType::CT_PT)
			{
				ACellBase* Cell = GetWorld()->SpawnActor<ACellBase>(CellHiddenClass, Transform, SpawnParams);
				Cell->SetCellType(ECellType::CT_PT);
				HiddenCells.Add(Cell);
				CellArray.Add(Cell);
				Cell->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
				CellBaseLocation.Add(FVector2D(Locations[i].X, Locations[i].Y));
			}
			else
			{

				CellArray.Add(nullptr);
				CellBaseLocation.Add(FVector2D(Locations[i].X, Locations[i].Y));
			}
		}
		if (CellCornerArray.Num() >= 4)
		{
			ACellCorner* CornerCell = Cast<ACellCorner>(CellCornerArray[0]);
			if (CornerCell)
			{
				CornerCell->AddHiddenCell(HiddenCells[0]);
				CornerCell->AddHiddenCell(HiddenCells[1]);
				CornerCell->AddHiddenCell(HiddenCells[4]);
				CornerCell->AddHiddenCell(HiddenCells[5]);
			}
			CornerCell = Cast<ACellCorner>(CellCornerArray[1]);
			if (CornerCell)
			{
				CornerCell->AddHiddenCell(HiddenCells[2]);
				CornerCell->AddHiddenCell(HiddenCells[3]);
				CornerCell->AddHiddenCell(HiddenCells[6]);
				CornerCell->AddHiddenCell(HiddenCells[7]);
			}
			CornerCell = Cast<ACellCorner>(CellCornerArray[2]);
			if (CornerCell)
			{
				CornerCell->AddHiddenCell(HiddenCells[8]);
				CornerCell->AddHiddenCell(HiddenCells[9]);
				CornerCell->AddHiddenCell(HiddenCells[12]);
				CornerCell->AddHiddenCell(HiddenCells[13]);
			}
			CornerCell = Cast<ACellCorner>(CellCornerArray[3]);
			if (CornerCell)
			{
				CornerCell->AddHiddenCell(HiddenCells[10]);
				CornerCell->AddHiddenCell(HiddenCells[11]);
				CornerCell->AddHiddenCell(HiddenCells[14]);
				CornerCell->AddHiddenCell(HiddenCells[15]);
			}
		}
		

	}
	SetGenerate(true);
	//SpawnFlags();
}
bool AGridGenerator::GenerationFinished_Validate()
{
	return true;
}
void AGridGenerator::RegenerationFinished_Implementation()
{

	if (RegenerateConfigRef)
	{
		FGridRegeneratorInfo RegenerateConfig = *RegenerateConfigRef;
		//UE_LOG(LogTemp, Warning, TEXT("RegenerationFinished %d"), RegenerateConfig.CellDisabledArray.Num());
		for (int32 i=0;i< RegenerateConfig.CellDisabledArray.Num();++i)
		{
			
			/*
			if (RegenerateConfigRef->CellDisabledArray[i])
			{
				RegenerateConfigRef->CellDisabledArray[i]->SetIsWalkable(false);
				RegenerateConfigRef->Causer->ChangeCellHighlight(RegenerateConfigRef->CellDisabledArray[i], false, EHighlightState::HC_Inactive);
			}
			*/
		}
		SetRegenerate(true);
	}
	
}
bool AGridGenerator::RegenerationFinished_Validate()
{
	return true;
}
void AGridGenerator::Generate_Implementation()
{
	if (CellBaseClass)
	{

		GridConfig.Interval = GridConfig.Interval*MapScale;

		GridConfig.Start = GetActorLocation();
		FVector Origin;
		FActorSpawnParameters SpawnParams;
		FTransform Transform(GetActorRotation(), GetActorLocation(), FVector(1.0f, 1.0f, 1.0f)*MapScale);
		ACellBase* Cell = GetWorld()->SpawnActor<ACellBase>(CellBaseClass,Transform, SpawnParams);
		Cell->GetActorBounds(false, Origin, GridConfig.CellSize);
		Cell->Destroy();
		GridConfigRef = &GridConfig;
		if (MapInfo.GridTypeName.IsEmpty() || MapInfo.GridTypeName == "Default")
		{
			FGridGeneratorAlg::JoyInit(GridConfigRef);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("CVSGeneration"));
			GridConfigRef->GridTypeName = MapInfo.GridTypeName;

			FGridGeneratorCSVAlg::JoyInit(GridConfigRef);
		}
		SetGenerate(false);	
	}
	
	
}
bool AGridGenerator::Generate_Validate()
{
	return true;
}
void AGridGenerator::BeginPlay()
{
	Super::BeginPlay();
	

}
void AGridGenerator::UpgradeGridConfigServer_Implementation()
{
	GridConfig = *GridConfigRef;
}
bool AGridGenerator::UpgradeGridConfigServer_Validate()
{
	return true;
}
void AGridGenerator::SetMapInfo(const FMapInfo& Value)
{
	MapInfo = Value;
}
void AGridGenerator::SetMapScale(float Value)
{
	MapScale = Value;

	SetMapScaleMulticast(Value);
}
void AGridGenerator::SetMapScaleMulticast_Implementation(float Value)
{
	MapScale = Value;
}
bool AGridGenerator::SetMapScaleMulticast_Validate(float Value)
{
	return true;
}
void AGridGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Role == ROLE_Authority)
	{
		if (FGridGeneratorAlg::IsThreadFinished() && bGenerate == false)
		{


			FGridGeneratorAlg::Shutdown();
			UpgradeGridConfigServer();
			GenerationFinished();

		}
		if (FGridGeneratorCSVAlg::IsThreadFinished() && bGenerate == false)
		{


			FGridGeneratorCSVAlg::Shutdown();
			UpgradeGridConfigServer();
			GenerationFinished();

		}
		if (FRegenerateGrid::IsThreadFinished() && bRegenerate == false)
		{
			FRegenerateGrid::Shutdown();

			if (Role == ROLE_Authority)
				RegenerationFinished();
		}
	}
}
void AGridGenerator::SetGenerate_Implementation(bool Value)
{
	bGenerate = Value;
}
bool AGridGenerator::SetGenerate_Validate(bool Value)
{
	return true;
}
void AGridGenerator::SetRegenerate_Implementation(bool Value)
{
	bRegenerate = Value;
}
bool AGridGenerator::SetRegenerate_Validate(bool Value)
{
	return true;
}
TArray<class ACellBase*> AGridGenerator::GetNeighbours(ACellBase * Cell)
{
	
	TArray<ACellBase*> Neighbours;
	//int32 CellIndex = GridConfig.Locations.Find(FVector2D(Cell->GetActorLocation()));
	int32 CellIndex = CellArray.Find(Cell);
	int32 CellColumn = CellIndex % GridConfig.NumOfColumns;
	int32 CellRow = CellIndex / GridConfig.NumOfColumns;
	if (Cell->GetCellType()!=ECellType::CT_Corner)
	{
		for (int8 x = -1; x <= 1; ++x)
		{

			for (int8 y = -1; y <= 1; ++y)
			{

				if ((x == 0 && y == 0) || (x == -1 && y != 0) || (x == 1 && y != 0))
					continue;


				
				int32 Index = x * GridConfig.NumOfColumns + y + CellIndex;
				

				if (Index > 0 && Index < GridConfig.Locations.Num())
				{
					
					if (Cell->GetCellType() == ECellType::CT_PT)
					{
						if (GridConfig.CellTypes[Index] != ECellType::CT_None&&GridConfig.CellTypes[Index] != ECellType::CT_PT&&GridConfig.CellTypes[Index] != ECellType::CT_Corner)
						{	
							
							if (CellArray[Index])
							{
								if(FVector::Dist(Cell->GetActorLocation(), CellArray[Index]->GetActorLocation())<GridConfig.CellSize.X*5)
								Neighbours.Add(CellArray[Index]);

							}
						}
					}
					else
					{
						if (GridConfig.CellTypes[Index] != ECellType::CT_None&&GridConfig.CellTypes[Index] != ECellType::CT_PT)
						{
							if (CellArray[Index])
							{
								if (FVector::Dist(Cell->GetActorLocation(), CellArray[Index]->GetActorLocation()) < GridConfig.CellSize.X * 5)
								Neighbours.Add(CellArray[Index]);

							}
						}
						else if (GridConfig.CellTypes[Index] == ECellType::CT_PT&&Cell->GetCellType() != ECellType::CT_PT)
						{
							if (FVector2D::Distance(FVector2D(Cell->GetActorLocation()), GridConfig.Locations[Index]) < GridConfig.CellSize.X * 5)
							{
								float MinDistance = MAX_FLT;
								int32 MinI = MAX_int32;
								for (int32 i = CellArray.Num() - 4; i < CellArray.Num(); ++i)
								{
						
									float Distance = FVector2D::Distance(FVector2D(CellArray[i]->GetActorLocation()), GridConfig.Locations[Index]);
									if (MinDistance > Distance)
									{
										MinDistance = Distance;
										MinI = i;
									}
								}
								if (MinI != MAX_int32)
								{
									if (CellArray[MinI])
										Neighbours.Add(CellArray[MinI]);
								}
							}
							
						}
					}
					
				}


			}
		}
	}
	else 
	{
		ACellCorner* CornerCell = Cast<ACellCorner>(Cell);
		if (CornerCell)
		{
			for (ACellBase* CellB : CornerCell->GetHiddenCells())
			{
				if(CellB->GetCellType()==ECellType::CT_PT)
				Neighbours.Append(GetNeighbours(CellB));
			}
			
		}
	}
	
	
	return Neighbours;
}
void AGridGenerator::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGridGenerator, bGenerate);
	DOREPLIFETIME(AGridGenerator, bRegenerate);
	DOREPLIFETIME(AGridGenerator, GridConfig);
	
	DOREPLIFETIME(AGridGenerator, MapInfo);
	
	DOREPLIFETIME(AGridGenerator, CellBaseArray);
	DOREPLIFETIME(AGridGenerator, CellBaseLocation);
	DOREPLIFETIME(AGridGenerator, CellDisabledArray);
	DOREPLIFETIME(AGridGenerator, CellDisabledLocations);
	DOREPLIFETIME(AGridGenerator, CellAttackArray);
	DOREPLIFETIME(AGridGenerator, CellDefenseArray);
	DOREPLIFETIME(AGridGenerator, CellArray);
	DOREPLIFETIME(AGridGenerator, StartingCellArray);
	DOREPLIFETIME(AGridGenerator, CellCornerArray);
	DOREPLIFETIME(AGridGenerator, DisabledQuadrants);
	
}

void AGridGenerator::SetPlayerStart()
{
		TArray<AActor*> Cameras;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("CameraStart"), Cameras);

	
		
		TArray<AActor*> Actors;
		TArray<ACellBase*> TempCellArray = StartingCellArray;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInvadedCharacter::StaticClass(), Actors);

		for (AActor* Actor : Actors)
		{
			AInvadedCharacter* Player = Cast<AInvadedCharacter>(Actor);
			if (Player&&TempCellArray.Num())
			{
				AInvadedPlayer* PlayerCharacter = Cast<AInvadedPlayer>(Player);
				if (PlayerCharacter&&Cameras.Num())
				{

					PlayerCharacter->SetCameraRotation(Cameras[0]->GetActorRotation());
					PlayerCharacter->SetCameraLocation(Cameras[0]->GetActorLocation());
				}

				if (Player->GetOversteppedCell())
				{
					Player->GetOversteppedCell()->SetIsOccupied(false);
					Player->GetOversteppedCell()->SetOccupyingPlayer(nullptr);
				}
				FVector Location;


				Location = TempCellArray[0]->GetActorLocation()+ FVector(0.0f, 0.0f, 100.0f)*MapScale;
				
				ScalePlayer(Player,MapScale);

				Player->TeleportPlayer(Player, Location);
				Player->SetStartingCell(TempCellArray[0]);
				Player->SetOversteppedCell(TempCellArray[0]);
				
				TempCellArray[0]->SetIsOccupied(true);
				TempCellArray[0]->SetOccupyingPlayer(Player);
				TempCellArray.RemoveAt(0);
			}
		}
		Actors.Empty();
		TempCellArray = StartingCellArray;
		
}
void AGridGenerator::SetActorLocation_Implementation(FVector Location, AActor* Actor)
{
	FRotator Rotator = FRotator(0.0f, 0.0f, 0.0f);
	if (Actor->GetNetMode() == ENetMode::NM_ListenServer)
		Location -= FVector(0.0f, 0.0, 100.0f);
	Actor->TeleportTo(Location,Rotator);
}
bool AGridGenerator::SetActorLocation_Validate(FVector Location, AActor* Actor)
{
	return true;
}
void AGridGenerator::ScalePlayer_Implementation(AInvadedCharacter* Player,float Scale)
{
	if(Player && (Scale > 0.0f))
	Player->SetActorScale3D(FVector(1.0f, 1.0f, 1.0f)*Scale);
}
bool AGridGenerator::ScalePlayer_Validate(AInvadedCharacter* Player, float Scale)
{
	return true;
}
void AGridGenerator::SpawnFlags()
{
	TArray<ACellCorner*> TempArray = CellCornerArray;
	TArray<ACellBase*> TempStartingCells = StartingCellArray;
	FActorSpawnParameters SpawnParams;
	TArray<AFlag*> FlagArray;
	AInvadedGS* InvadedGS = Cast<AInvadedGS>(UGameplayStatics::GetGameState(GetWorld()));

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInvadedCharacter::StaticClass(), Actors);
	
	if (InvadedGS)
	{
		while (TempArray.Num()&&(FlagArray.Num()< InvadedGS->GetNumberOfPlayers()))
		{
			FTransform Transform(FRotator(0.0f, 0.0f, 0.0f), TempArray[0]->GetActorLocation() + FVector(0.0f, 0.0f, GridConfig.CellSize.Z), FVector(1.0f, 1.0f, 1.0f)*MapScale);
			AFlag* Flag = GetWorld()->SpawnActor<AFlag>(FlagClass,Transform, SpawnParams);
			Flag->SetLastOversteppedCell(TempArray[0]);
			Flag->SetDeathCell(TempStartingCells[0]);
			Flag->SetCornerCell(TempArray[0]);
			TempStartingCells.RemoveAt(0);
			TempArray.RemoveAt(0);
			FlagArray.Add(Flag);
		}
		UE_LOG(LogTemp, Warning, TEXT("FLAGS %d"), FlagArray.Num() - InvadedGS->GetNumberOfPlayers());
		AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
		if (InvadedGM)
		{
			InvadedGM->ConfigurateFlags(FlagArray);
		}
	}
	
}
void AGridGenerator::ResetFlags()
{
	TArray<AActor*> FlagArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFlag::StaticClass(), FlagArray);
	for (int32 i = 0; i < FlagArray.Num(); ++i)
	{
		AFlag* Flag = Cast<AFlag>(FlagArray[i]);
		if (Flag)
		{
			Flag->RemoveFromPlayer();
			Flag->SetActorLocation(CellCornerArray[i]->GetActorLocation());
			Flag->SetLastOversteppedCell(CellCornerArray[i]);
		}
	}
}

void AGridGenerator::RegenerateSpecialCellsAsync()
{
	TArray<ACellBase*> NewlyCreatedCells;
	TArray < int32> Indexes;
	FActorSpawnParameters SpawnParams;
	for (int32 i = 0; i < CellAttackArray.Num(); ++i)
	{
		ACellBase* Cell = CellAttackArray[i];
		if (CellDisabledLocations.Find(FVector(FVector2D(Cell->GetActorLocation()), GridConfig.Start.Z)) == INDEX_NONE)
		{
			
			ACellBase* NewCell = GetWorld()->SpawnActor<ACellBase>(CellBaseClass, Cell->GetActorTransform(), SpawnParams);
			NewCell->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
			CellBaseArray.Add(NewCell);
			
			NewlyCreatedCells.Add(NewCell);
			Indexes.Add(CellArray.Find(Cell));
			Cell->Destroy();
			
		}
	}
	
	
	CellAttackArray.Empty();
	for (int32 i = 0; i < CellAttackArray.Num(); ++i)
	{
		ACellBase* Cell = CellDefenseArray[i];
		if (CellDisabledLocations.Find(FVector(FVector2D(Cell->GetActorLocation()), GridConfig.Start.Z)) == INDEX_NONE)
		{
			
			ACellBase* NewCell = GetWorld()->SpawnActor<ACellBase>(CellBaseClass, Cell->GetActorTransform(), SpawnParams);
			NewCell->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
			CellBaseArray.Add(NewCell);
		
			NewlyCreatedCells.Add(NewCell);
			Indexes.Add(CellArray.Find(Cell));
			Cell->Destroy();
			
			
		}

	}
	CellDefenseArray.Empty();
	
	for (ECellType CellType : GridConfigRef->CellTypes)
	{
		if (CellType == ECellType::CT_Attack || CellType == ECellType::CT_Defense)
			CellType = ECellType::CT_Base;
	}

	GenerateAttackCells(GridConfigRef);
	GenerateDefenseCells(GridConfigRef);
	
	UpgradeGridConfigServer();
	
	TArray<FVector2D> Locations = GridConfig.Locations;
	TArray<ECellType> CellTypes = GridConfig.CellTypes;
	TArray<ACellBase*> HiddenCells;
	TArray<class ACellBase*> NewCellBaseArray;
	int32 Cnt = 0;
	
	for (uint8 i = 0; i < Locations.Num(); ++i)
	{
		bool bChecker = true;
		for (int32 Index : DisabledQuadrants)
		{
			if (GridConfig.Quadrants.Num() > Index && Index >=0)
			{
				if (GridConfig.Quadrants[Index].Array.Find(Locations[i]) != INDEX_NONE)
				{
					bChecker = false;
					break;

				}
			}
			
		}
		if (bChecker)
		{
			if (CellTypes[i] == ECellType::CT_Attack || CellTypes[i] == ECellType::CT_Defense)
			{
				for (ACellBase* Cell : CellBaseArray)
				{
					if (Cell)
					{
						if (FVector2D(Cell->GetActorLocation().X, Cell->GetActorLocation().Y) == FVector2D(Locations[i]))
						{
							Cnt++;
							Cell->Destroy();
						}
						else
						{
							NewCellBaseArray.AddUnique(Cell);
						}
							
					}
					
				}
				
				if (CellTypes[i] == ECellType::CT_Attack)
				{
					FTransform Transform(GetActorRotation(), FVector(Locations[i].X, Locations[i].Y, GetActorLocation().Z), FVector(1.0f, 1.0f, 1.0f)*MapScale);
					ACellAttack* Cell = GetWorld()->SpawnActor<ACellAttack>(CellAttackClass,Transform, SpawnParams);
					CellAttackArray.Add(Cell);
					Cell->SetCellType(ECellType::CT_Attack);
					Cell->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
					CellArray[i]=Cell;
					Cell->SetIsWalkable(true);
					Cell->SetIsFocusable(true);
					
				}
				else if (CellTypes[i] == ECellType::CT_Defense)
				{
					FTransform Transform(GetActorRotation(), FVector(Locations[i].X, Locations[i].Y, GetActorLocation().Z), FVector(1.0f, 1.0f, 1.0f)*MapScale);
					ACellDefense* Cell = GetWorld()->SpawnActor<ACellDefense>(CellDefenseClass,Transform, SpawnParams);
					CellDefenseArray.Add(Cell);
					Cell->SetCellType(ECellType::CT_Defense);
					Cell->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
					CellArray[i]=Cell;
					Cell->SetIsWalkable(true);
					Cell->SetIsFocusable(true);
					
				}
				
				CellBaseArray = NewCellBaseArray;
			}

			
		}
		
	}
	
	
	for (int32 i = 0; i < Indexes.Num()&&i<NewlyCreatedCells.Num(); ++i)
	{
		if (NewlyCreatedCells[i])
		{
		//	CellArray[Indexes[i]] = NewlyCreatedCells[i];
		}
	}
	
}
void AGridGenerator::RegenerateSpecialCells()
{
	
	AsyncTask(ENamedThreads::HighTaskPriority, [=]()
	{
		//RegenerateSpecialCellsAsync();
	});
	
}
void AGridGenerator::DisableQuadrant(ACellBase * StartingCell, class AInvadedCharacter* Causer)
{
	AsyncTask(ENamedThreads::GameThread, [=]()
	{
		DisableQuadrantAsync(StartingCell, Causer);
		//RegenerateSpecialCellsAsync();
		
	});
}
void AGridGenerator::DisableQuadrantAsync(ACellBase * StartingCell, class AInvadedCharacter* Causer)
{

	FVector2D StartingLocation = FVector2D(StartingCell->GetActorLocation().X, StartingCell->GetActorLocation().Y);
	int32 Index=StartingCellArray.Find(StartingCell);
	int32 Cnt = 0;
	Causer->ClearSelections();
	Causer->SetPlayerMode(EPlayerMode::PM_UI);
	//CellDisabledArray.Add(StartingCell);
	DisabledQuadrants.Add(Index);



	if (GridConfig.Quadrants.Num()>Index&&Index>-1)
	{
		
		for (FVector2D Location : GridConfig.Quadrants[Index].Array)
		{
			
			for (int32 i=0;i<CellArray.Num();++i)
			{
				ACellBase* Cell = CellArray[i];
				if (Cell)
				{
					if (Cell->GetCellType() != ECellType::CT_StartingPoint)
					{
	
						if (FVector2D(CellBaseLocation[i]-Location).Size()<100)
						{
							CellDisabledLocations.Add(FVector(FVector2D(Cell->GetActorLocation().X, Cell->GetActorLocation().Y), GridConfig.Start.Z));
							
							Cell->SetIsWalkable(false);

							Cell->SetIsFocusable(false);

							if (IsNetMode(ENetMode::NM_Standalone))
							{				
								Cell->SetHighlightState(EHighlightState::HC_Inactive);
								Cell->OnBeginFocus();
							}
							else
							{
								Cell->SetHighlightState(EHighlightState::HC_Inactive);
								Causer->ChangeCellHighlight(Cell, true, EHighlightState::HC_Inactive);
							}

							CellDisabledArray.Add(Cell);
							break;
						}
					}
					
				}
				
			}
			for (ACellCorner* CellCorner : CellCornerArray)
			{
				if (CellCorner)
				{
					for (ACellBase* Cell : CellCorner->GetHiddenCells())
					{
						if (FVector2D(Cell->GetActorLocation().X, Cell->GetActorLocation().Y) == Location)
						{
							CellDisabledLocations.Add(FVector(FVector2D(CellCorner->GetActorLocation().X, CellCorner->GetActorLocation().Y), GridConfig.Start.Z));

							CellCorner->SetIsWalkable(false);
							CellCorner->SetIsFocusable(false);

							if (IsNetMode(ENetMode::NM_Standalone))
							{
								Cell->SetHighlightState(EHighlightState::HC_Inactive);
								Cell->OnBeginFocus();
							}
							else
							{
								Causer->ChangeCellHighlight(Cell, true, EHighlightState::HC_Inactive);
							}

							CellDisabledArray.Add(CellCorner);
							CellDisabledArray.Append(CellCorner->GetHiddenCells());
						
							break;
						}

					}
				}
			}
		}
	}
	Causer->SetPlayerMode(EPlayerMode::PM_Base);
	RegenerateSpecialCellsAsync();
	Causer->NextPathCell();

}

void AGridGenerator::GenerateAttackCells(FGridGeneratorInfo * Data)
{
	uint8 j = 0;
	while (j < Data->NumOfAttackCells)
	{
		int Index = FMath::RoundToInt(FMath::FRandRange(0.0f, Data->Locations.Num() - 1));
		if (Data->CellTypes[Index] == ECellType::CT_Base&&ClearNeighbourhood(Data, Index, Data->CheckRange, ECellType::CT_Attack))
		{
			Data->CellTypes[Index] = ECellType::CT_Attack;
			j++;
		}

	}
}

void AGridGenerator::GenerateDefenseCells(FGridGeneratorInfo * Data)
{
	uint8 j = 0;
	while (j < Data->NumOfDefenseCells)
	{
		int Index = FMath::RoundToInt(FMath::FRandRange(0.0f, Data->Locations.Num() - 1));
		if (Data->CellTypes[Index] == ECellType::CT_Base&&ClearNeighbourhood(Data, Index, Data->CheckRange, ECellType::CT_Defense))
		{
			Data->CellTypes[Index] = ECellType::CT_Defense;
			j++;
		}

	}
}

bool AGridGenerator::ClearNeighbourhood(FGridGeneratorInfo * Data, uint8 Index, uint8 Range, ECellType CellType)
{
	/*
	for (uint8 i : GetNeighboursIndex(Data, Index, false))
	{
		if (Data->CellTypes[i] == ECellType::CT_Defense || Data->CellTypes[i] == ECellType::CT_Attack)
		{
			return false;
		}
		if (Range > 1)
		{
			
			bool ReturnValue = ClearNeighbourhood(Data, i, Range - 1, CellType);
			if (!ReturnValue)
				return false;
		}

	}
	*/
	return true;
}

TArray<uint8> AGridGenerator::GetNeighboursIndex(FGridGeneratorInfo * Data, uint8 Index, bool bOnlyBlank)
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
