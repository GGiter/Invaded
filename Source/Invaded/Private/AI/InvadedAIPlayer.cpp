// Fill out your copyright notice in the Description page of Project Settings.

#include "InvadedAIPlayer.h"
#include "InvadedMovementComp.h"
#include "InvadedData.h"
#include "InvadedGS.h"
#include "Kismet/GameplayStatics.h"
#include "Environment/GridGenerator.h"
#include "TimerManager.h"
#include "Actors/CellBase.h"
#include "Actors/Cards/CardBase.h"
#include "Actors/CellAttack.h"
#include "Actors/CellDefense.h"
#include "GameModes/InvadedGM.h"
#include "Components/CapsuleComponent.h"
#include "Actors/Dice.h"
#include "Alg/FAStarNT.h"
#include "AI/AICardManager.h"
#include "Actors/CellCorner.h"
#include "Player/InvadedPlayer.h"
#include "FightGame.h"


// Sets default values
AInvadedAIPlayer::AInvadedAIPlayer(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UInvadedMovementComp>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bIsCurrentlyWorking = false;
	bReplicates = true;

	GetMesh()->SetCustomDepthStencilValue(250);

	CustomCellRatio = 0.0f;

	AICardManager = CreateDefaultSubobject<UAICardManager>(TEXT("AICardManager"));

	bDicePodReady = false;

	
}

void AInvadedAIPlayer::SetupPlayer(FString PlayerName,int32 Number)
{
	HighlightDelegate.BindUFunction(this, FName("FindPath"), OversteppedCell, LastFocusedCell);

	
	UE_LOG(LogTemp, Warning, TEXT("Setting up AIPlayer"));

	if (GetInvadedPS())
	{
		InvadedPS->SetPlayerName(PlayerName);
		InvadedPS->SetAIPlayerID(Number);
		if (ColorPool.Num())
		{
			FLinearColor C1 = ColorPool[FMath::RandRange(0, ColorPool.Num() - 1)];
			FLinearColor C2 = C1;
			while (C1 == C2)
			{
				C2 = ColorPool[FMath::RandRange(0, ColorPool.Num() - 1)];
			}

			InvadedPS->SetPlayerColor(C1);
			InvadedPS->SetFlagColor(C2);
		}
	
	}

	TArray<FHitResult> OutHitArray;

	for (FHitResult OutHit : OutHitArray)
	{
		if (OutHit.GetActor())
		{
			ACellBase* Cell = Cast<ACellBase>(OutHit.GetActor());
			if (Cell)
			{
				if (Cell&&OversteppedCell)
				{

					SetOversteppedCell(Cell);


				}

			}
		}
	}
	
}

void AInvadedAIPlayer::NextTurn(class UInvadedData* CurrentPS)
{
	
	if (InvadedPS)
	{
		InvadedPS->SetPlayerMode(EPlayerMode::PM_Base);
		//FindOversteppedCell();
	}
	
	if (InvadedPS)
	{
		
		if (InvadedPS->GetIsCurrentPlayer())
		{
			UE_LOG(LogTemp, Warning, TEXT("THROW DICE!!!"));
			bIsCurrentlyWorking = true;
			YourFlag = FindYourFlag();
			PlayTurn();
		}
	}
}

void AInvadedAIPlayer::CallTurnEnd()
{
	if ((InvadedPS->GetRemainingMoves() == 0&&InvadedPS->GetIsCurrentPlayer()==false)|| (InvadedPS->GetRemainingMoves() == 0 && InvadedPS->GetIsCurrentPlayer() == true&&InvadedPS->HasThrownDice()))
	{

		GetInvadedMovementComp()->EndFight();

		bIsCurrentlyWorking = false;
		bHasFoundPath = false;

		UE_LOG(LogTemp, Warning, TEXT("AI is ending it's turn. %s"),*GetName());

		Super::CallTurnEnd();
	}
}

void AInvadedAIPlayer::EnterFightMode(AActor * Enemy)
{
	Super::EnterFightMode(Enemy);	

	
}

void AInvadedAIPlayer::FightWon_Implementation(AInvadedCharacter* Enemy)
{
	Super::FightWon_Implementation(Enemy);

}

void AInvadedAIPlayer::FightLost_Implementation(AInvadedCharacter* Enemy)
{
	Path.Empty();
	UE_LOG(LogTemp, Warning, TEXT("Fight lost"));
	Super::FightLost_Implementation(Enemy);
	
}

void AInvadedAIPlayer::FightDraw_Implementation(AInvadedCharacter* Enemy)
{
	Path.Empty();
	UE_LOG(LogTemp, Warning, TEXT("Fight draw"));
	Super::FightDraw_Implementation(Enemy);
}

// Called when the game starts or when spawned
void AInvadedAIPlayer::BeginPlay()
{
	Super::BeginPlay();

	if (Role == ROLE_Authority)
	{
		bAggressive = FMath::FRand() < 0.5f ? true : false;
	}
	
}



// Called every frame
void AInvadedAIPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	

}
void AInvadedAIPlayer::PlayTurn()
{

	if (Role == ROLE_Authority)
	{
		FindOversteppedCell();
		GetInvadedPS()->SetPlayerMode(EPlayerMode::PM_Base);
		GetInvadedPS()->SetEnemyAction(EEnemyAction::EA_Base);

		MinimalMoves = 0;

		DetermineRatios();

		AICardManager->ChooseCards();		
	}
	

}

void AInvadedAIPlayer::FinishFightMode()
{
	AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (InvadedGM)
	{
		InvadedGM->SetPlayerFightValue(this, FMath::RandRange(1, 6));
	}
}
void AInvadedAIPlayer::DetermineRatios()
{
	if (!DoesAnyoneHaveYourFlag() && !DoYouHaveEnemyFlag())
	{
		PlayerRatio = 0.1f;
		FlagRatio = FMath::FRandRange(0.25f,1.0f);
		DefenseCellRatio = FMath::FRandRange(0.25f, 1.0f);
		AttackCellRatio = FMath::FRandRange(0.25f, 1.0f);

		if (!bAggressive&&YourFlag)
		{
			//FlagRatio = 0.5f;
			//CustomCellRatio = 10.0f;

			//DesiredCell = YourFlag->GetLastOversteppedCell();
		}
	}
	else if (DoesAnyoneHaveYourFlag())
	{
		DesiredPlayer = DoesAnyoneHaveYourFlag();
		PlayerRatio = 1.0f;
		FlagRatio = 0.0f;
		DefenseCellRatio = 0.25f;
		AttackCellRatio = 0.25;
	}
	else if (DoYouHaveEnemyFlag())
	{
		PlayerRatio = 0.1f;
		FlagRatio = 0.1f;
		DefenseCellRatio = 0.1f;
		AttackCellRatio = 0.1f;

		CustomCellRatio = 10.0f;

		AFlag* Flag = DoYouHaveEnemyFlag();

		DesiredCell = Flag->GetDeathCell();

	}
	if (bAttackPlayer)
	{
		DesiredPlayer = nullptr;
		DesiredCell = nullptr;
		PlayerRatio = 1.0f;
		FlagRatio = 0.1f;
		DefenseCellRatio = 0.1f;
		AttackCellRatio = 0.1f;

	}
}
int32 AInvadedAIPlayer::FindDistanceTo(ACellBase* StartCell, AActor * Actor)
{
	int32 Distance = 0;
	ACellBase* Cell = nullptr;
	AInvadedCharacter* Player = Cast<AInvadedCharacter>(Actor);
	if (Player)
	{
		Cell = Player->GetOversteppedCell();
	}
		
	AFlag* Flag = Cast<AFlag>(Actor);
	if (Flag)
	{
		Cell = Flag->GetLastOversteppedCell();
	}
	
	if(Cell==nullptr)
	Cell = Cast<ACellBase>(Actor);
	
	AInvadedGS* InvadedGS = Cast<AInvadedGS>(UGameplayStatics::GetGameState(GetWorld()));
	if (InvadedGS)
	{
		AGridGenerator* GridGenerator = InvadedGS->GetGridGenerator();
		if (GridGenerator&&StartCell&&Cell)
		{
			//return UFAStarNT::GetPath(StartCell, Cell, GridGenerator).Num();
			//too expensive

			return FVector2D::Distance(FVector2D(StartCell->GetActorLocation()), FVector2D(Cell->GetActorLocation()));
		}
	}
		
	
	return Distance;
}
void AInvadedAIPlayer::GameWon()
{
	Super::GameWon();
	bIsCurrentlyWorking = true;
}
void AInvadedAIPlayer::ToggleDicePod()
{
	bDicePodReady = !bDicePodReady;
}
float AInvadedAIPlayer::DetermineValue(class ACellBase * Cell)
{
	if (bAvoidPlayer)
	{
		if (Cell->GetIsOccupied())
			return FLT_MAX-1;
	}
	if (FindClosestActor(AInvadedCharacter::StaticClass()))
	{
		float PlayerValue = -PlayerRatio * FindDistanceTo(Cell,FindClosestActor(AInvadedCharacter::StaticClass()));
		float FlagValue = -FlagRatio * FindDistanceTo(Cell,FindClosestActor(AFlag::StaticClass()));
		float DefenseCellValue = -DefenseCellRatio *FindDistanceTo(Cell,FindClosestActor(ACellDefense::StaticClass()));
		float AttackCellValue = -AttackCellRatio * FindDistanceTo(Cell,FindClosestActor(ACellAttack::StaticClass()));

		if (bAttackHumanPlayer)
		{
			return -PlayerRatio * FindDistanceTo(Cell, FindClosestActor(AInvadedPlayer::StaticClass()));
		}

		if(DesiredPlayer)
			return -PlayerRatio * FindDistanceTo(Cell,DesiredPlayer);
		if (DesiredCell)
			return  -CustomCellRatio * FindDistanceTo(Cell,DesiredCell);

		
		return FMath::Min(FMath::Min(PlayerValue, FlagValue), FMath::Min(DefenseCellValue, AttackCellValue));
	}
	else
	{
		return FLT_MAX-1;
	}
	
}
float AInvadedAIPlayer::DeterminePlayerValue(AInvadedCharacter * Player)
{
	float Value = 0;
	if (DoesPlayerHaveYourFlag(Player))
		Value = -1;
	
	return Value;
}
void AInvadedAIPlayer::ChooseEnemyAction()
{
	InvadedPS->SetPlayerMode(EPlayerMode::PM_Base);

	switch (InvadedPS->GetEnemyAction())
	{
		case EEnemyAction::EA_LoseBattle:
		{
			AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
			if (InvadedGM)
			{
				InvadedGM->AddNextBattleLoser(SelectedPlayer);
			}

			break;


		
		}
		case EEnemyAction::EA_LoseTurn:
		{
			AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
			if (InvadedGM)
			{
				InvadedGM->SkipPlayerTurn(SelectedPlayer);
			
			}

		
			break;
		}
		case EEnemyAction::EA_Move:
		{
			SelectedPlayer->ProjectPath(2, CalculatePath(SelectedPlayer,2));

			InvadedPS->SetEnemyAction(EEnemyAction::EA_Base);


			break;
		}
		case EEnemyAction::EA_MoveToCornerCell:
		{
			InvadedPS->SetPlayerMode(EPlayerMode::PM_CornerCellSelection);
			InvadedPS->SetEnemyAction(EEnemyAction::EA_MoveToCornerCell);

	

			break;
		}
	}


	LastFocusedPlayer = SelectedPlayer;

	ClearSelections();

	ChoosePlayerMode();
}
TArray<ACellBase*> AInvadedAIPlayer::CalculatePath(AInvadedCharacter * Player, int32 Distance)
{
	TArray<ACellBase*> OutArray;
	AInvadedGS* InvadedGS = Cast<AInvadedGS>(UGameplayStatics::GetGameState(GetWorld()));
	if (InvadedGS)
	{
		if (InvadedGS->GetGridGenerator())
		{
			OutArray.Add(OversteppedCell);
			for (int32 j = 0; j < Distance; ++j)
			{
				TArray<ACellBase*> Neighbours = InvadedGS->GetGridGenerator()->GetNeighbours(OutArray.Last());
				TPair<int32, float> Min;
				Min.Key = -1;
				Min.Value = -10000000;

				for (int32 i = 0; i < Neighbours.Num(); ++i)
				{
					if (Neighbours[i] != nullptr)
					{
						if (Neighbours[i]->GetIsWalkable())
						{
							float Value = DetermineValue(Neighbours[i]);

							if (Value < Min.Value&&Path.Find(Neighbours[i]) == INDEX_NONE)
							{
								Min.Value = Value;
								Min.Key = i;
							}
						}
					}
				}
				if (Min.Key != -1)
				{
					OutArray.Add(Neighbours[Min.Key]);
				}
			}
		}
	}
	return OutArray;
}
void AInvadedAIPlayer::ChoosePlayerMode()
{
	
	UE_LOG(LogTemp, Warning, TEXT("Choosing Player Mode"));
	switch (InvadedPS->GetPlayerMode())
	{
	case EPlayerMode::PM_Base:
		{
			GetWorldTimerManager().SetTimer(FindPathHandle, HighlightDelegate, 0.5f, true);
			break;
		}
	case EPlayerMode::PM_CornerCellSelection:
		{
			SelectCornerCell();
			break;
		}
	case EPlayerMode::PM_PlayerSelection:
		{
			SelectPlayer();
			break;
		}
	case EPlayerMode::PM_Fight:
		{
			GetWorldTimerManager().SetTimer(FindPathHandle, HighlightDelegate, 0.5f, true);
			break;
		}
	}
	
}
void AInvadedAIPlayer::SelectCornerCell()
{
	InvadedPS->SetPlayerMode(EPlayerMode::PM_Base);
	AInvadedGM* InvadedGM = Cast<AInvadedGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (InvadedGM)
	{
		TArray<ACellCorner*> CornerCells=InvadedGM->GetGridGenerator()->GetCornerCells();
		TPair<int32, float> Min;
		Min.Key = -1;
		Min.Value = FLT_MAX;
		TPair<int32, float> Max;
		Max.Key = -1;
		Max.Value = -1;


		for (int32 i = 0; i < CornerCells.Num(); ++i)
		{
			if (CornerCells[i] != nullptr)
			{
				float Value = DetermineValue(CornerCells[i]);

				if (Value <= Min.Value)
				{

					Min.Value = Value;
					Min.Key = i;

				}
				if (Value > Max.Value)
				{

					Max.Value = Value;
					Max.Key = i;

				}
			}

		}
		if (Min.Key != -1)
		{
			SelectedCornerCell = CornerCells[Min.Key];
			if (InvadedPS->GetEnemyAction() == EEnemyAction::EA_Base)
			{
				FVector Origin, Extent;
				CornerCells[Min.Key]->GetActorBounds(false, Origin, Extent);
				TeleportPlayer(this, CornerCells[Min.Key]->GetActorLocation()+ FVector(0.0f, 0.0f, 100.0f)*AGridGenerator::GetMapScale());
				SetOversteppedCell(CornerCells[Min.Key]);
			}
			else if (SelectedPlayer != nullptr)
			{
				FVector Origin, Extent;
				CornerCells[Min.Key]->GetActorBounds(false, Origin, Extent);
				TeleportPlayer(SelectedPlayer, CornerCells[Min.Key]->GetActorLocation()+ FVector(0.0f, 0.0f, 100.0f)*AGridGenerator::GetMapScale());
				SelectedPlayer->SetOversteppedCell(CornerCells[Min.Key]);

				ChangePlayerHighlight(SelectedPlayer, false, EHighlightState::HC_Neutral);
			}
			
		}
	}
	ChoosePlayerMode();
}
void AInvadedAIPlayer::SelectPlayer()
{
	TArray<AActor*> Actors;
	TPair<int32, float> Min;
	Min.Key = -1;
	Min.Value = FLT_MAX-1;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInvadedCharacter::StaticClass(), Actors);
	for (int32 i = 0; i < Actors.Num(); ++i)
	{
		AInvadedCharacter* Char = Cast<AInvadedCharacter>(Actors[i]);
		if (Char&&Char!=this)
		{
			
			float Value = DeterminePlayerValue(Char);

			

			if (Value < Min.Value)
			{
				Min.Value = Value;
				Min.Key = i;
			}
		}

	}
	if (Min.Key != -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected Player %s"), *Actors[Min.Key]->GetName());
		SelectedPlayer = Cast<AInvadedCharacter>(Actors[Min.Key]);

		if (DesiredPlayer)
			SelectedPlayer = DesiredPlayer;

		LastFocusedPlayer = SelectedPlayer;
		ChangePlayerHighlight(SelectedPlayer, true, EHighlightState::HC_Positive);
	}
	ChooseEnemyAction();
}
AActor * AInvadedAIPlayer::FindClosestActor(UClass* ActorClass)
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ActorClass, Actors);
	TPair<int32, float> Min;
	Min.Key = -1;
	Min.Value = FLT_MAX-1;
	for (int32 i=0;i<Actors.Num();++i)
	{
		int32 Dist = FindDistanceTo(GetOversteppedCell(), Actors[i]);
		if (Dist < Min.Value&&Actors[i]!=this)
		{
			Min.Key = i;
			Min.Value = Dist;
		}
		
	}
	if (Min.Key != -1)
		return Actors[Min.Key];
	return nullptr;
}
bool AInvadedAIPlayer::DoesPlayerHaveYourFlag(AInvadedCharacter * Player)
{
	for (AFlag* Flag : Player->GetFlags())
	{
		if(InvadedPS&&Flag)
		if (Flag->GetOwnerName().ToString() == InvadedPS->GetPlayerName())
			return true;
	}
	return false;
}
AFlag * AInvadedAIPlayer::DoYouHaveEnemyFlag()
{
	for (AFlag* Flag : GetFlags())
	{
		if (Flag)
		{
			if (Flag->GetOwnerName().ToString() != InvadedPS->GetPlayerName())
				return Flag;
		}
		
	}
	return nullptr;
}
AFlag * AInvadedAIPlayer::FindYourFlag()
{
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFlag::StaticClass(), OutActors);
	for (AActor* Actor : OutActors)
	{
		AFlag* Flag = Cast<AFlag>(Actor);
		if (Flag)
		{
			if (InvadedPS&&Flag)
				if (Flag->GetFirstOwner() == this)
					return Flag;
		}
		
	}
	return nullptr;
}
void AInvadedAIPlayer::SetDesiredPlayer(AInvadedCharacter * Value)
{
	DesiredPlayer = Value;
}
AInvadedCharacter* AInvadedAIPlayer::DoesAnyoneHaveYourFlag()
{
	AInvadedCharacter* Answer = nullptr;
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInvadedCharacter::StaticClass(), OutActors);
	for (AActor* Actor : OutActors)
	{
		AInvadedCharacter* Player = Cast<AInvadedCharacter>(Actor);
		if (Player&&Player != this)
		{
			if (DoesPlayerHaveYourFlag(Player))
				Answer = Player;
		}
	}
	return Answer;
}
void AInvadedAIPlayer::FindPath(ACellBase* StartCell,ACellBase* EndCell)
{
	bool bCondition = true;
	if (SelectedPlayer)
	{
		if (SelectedPlayer->GetInvadedPS()->GetRemainingMoves() > 0 || SelectedPlayer->GetInvadedPS()->GetPlayerMode() == EPlayerMode::PM_Fight)
			bCondition = false;
		else
		{
			ChangePlayerHighlight(SelectedPlayer, false, EHighlightState::HC_Neutral);
			SelectedPlayer = nullptr;
		}
	}
	if(!bCondition)
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected player is moving"));
	}
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADice::StaticClass(), OutActors);
	if (OversteppedCell&&OutActors.Num() == 0&&bCondition&&InvadedPS->HasThrownDice())
	{
		UE_LOG(LogTemp, Warning, TEXT("FindPath %d"), InvadedPS->GetRemainingMoves());
		AInvadedGS* InvadedGS = Cast<AInvadedGS>(UGameplayStatics::GetGameState(GetWorld()));
		if (InvadedGS&&InvadedPS->GetRemainingMoves() && InvadedPS->GetRemainingMoves() > MinimalMoves)
		{
			Path.Empty();

			if (InvadedPS->GetRemainingMoves() <= 0)
			{
				GetWorldTimerManager().ClearTimer(FindPathHandle);
				CallTurnEnd();
			}
			if (InvadedGS->GetGridGenerator())
			{
				Path.Add(OversteppedCell);
				for (int32 j = 0; j < InvadedPS->GetRemainingMoves(); ++j)
				{
					TArray<ACellBase*> Neighbours = InvadedGS->GetGridGenerator()->GetNeighbours(Path.Last());
					TPair<int32, float> Min;
					Min.Key = -1;
					Min.Value = -FLT_MAX+1;

				
	
					for (int32 i = 0; i < Neighbours.Num(); ++i)
					{
						if (Neighbours[i] != nullptr)
						{
							if (Neighbours[i]->GetIsWalkable())
							{
								UE_LOG(LogTemp, Warning, TEXT("CellValue %f"), DetermineValue(Neighbours[i]));

								float Value = DetermineValue(Neighbours[i]);

								if (Value > Min.Value&&Path.Find(Neighbours[i]) == INDEX_NONE)
								{
									Min.Value = Value;
									Min.Key = i;

								}
							}
						
						}
						
					}
					if (Min.Key != -1)
					{
						UE_LOG(LogTemp, Warning, TEXT("CellValue %f"), Min.Value);
						Path.Add(Neighbours[Min.Key]);
					}
						
				}
				bHasFoundPath = true;
				if(Path.Num())
				Path.RemoveAt(0);
				LastPath = Path;
				RPath = Path;
				UE_LOG(LogTemp, Warning, TEXT("LastPath Size %d"), LastPath.Num());
				GetWorldTimerManager().ClearTimer(FindPathHandle);

				if (bSkipTurn)
				{
					CallTurnEnd();
				}
				else
				{
					RetraceThePath();
				}
				
			}
			
				

			
		}
	}	
}
void AInvadedAIPlayer::KilledPlayer()
{
	Super::KilledPlayer();

	GetWorldTimerManager().SetTimer(FindPathHandle, HighlightDelegate, 1.f, true);
}

void AInvadedAIPlayer::GameLost()
{
	

	if (GetWorld()->GetGameState<AInvadedGS>())
	{
		GetWorld()->GetGameState<AInvadedGS>()->RemoveInvadedPlayerState(GetInvadedPS());
		UInvadedData* NewInvadedData = NewObject<UInvadedData>(GetWorld()->GetGameState<AInvadedGS>());

		if (NewInvadedData)
		{
			NewInvadedData->SetPlayerColor(GetInvadedPS()->GetPlayerColor());
			NewInvadedData->SetFlagColor(GetInvadedPS()->GetFlagColor());
			NewInvadedData->SetPlayerName(GetInvadedPS()->GetPlayerName());
			NewInvadedData->SetTurnScore(GetInvadedPS()->GetTurnScore());
			
			GetWorld()->GetGameState<AInvadedGS>()->AddInvadedPlayerState(NewInvadedData);
		}

		
	}
	Super::GameLost();
	Destroy();
}

